#include <iostream>
#include <memory>
#include <optional>
#include <userver/server/server.hpp>

#include <atomic>
#include <stdexcept>

#include <engine/ev/thread_pool.hpp>
#include <engine/task/task_processor.hpp>
#include <server/handlers/http_handler_base_statistics.hpp>
#include <server/http/http_request_handler.hpp>
#include <server/http/http_request_impl.hpp>
#include <server/net/endpoint_info.hpp>
#include <server/net/listener.hpp>
#include <server/net/stats.hpp>
#include <server/requests_view.hpp>
#include <server/server_config.hpp>
#include <userver/engine/sleep.hpp>
#include <userver/logging/log.hpp>
#include <userver/utils/assert.hpp>
#include "server/net/tls_settings.hpp"
#include "userver/storages/secdist/component.hpp"
#include "userver/utils/trivial_map.hpp"

USERVER_NAMESPACE_BEGIN

namespace server {

namespace {
  enum class PortType { kMonitor, kMain, kSecure };

  constexpr utils::TrivialBiMap kPortTypeNamesMap = [](auto selector) {
    return selector()
        .Case(PortType::kMain, "main")
        .Case(PortType::kMonitor, "monitor")
        .Case(PortType::kSecure, "secure");
  };
}

class ServerImpl final {
 public:
  ServerImpl(ServerConfig config,
             const components::ComponentContext& component_context);
  ~ServerImpl();

  net::Stats GetServerStats() const;

  const ServerConfig config_;

  std::unique_ptr<RequestsView> requests_view_;


  struct PortInfo {
    std::shared_ptr<http::HttpRequestHandler> request_handler_;
    std::shared_ptr<net::EndpointInfo> endpoint_info_;
    request::ResponseDataAccounter data_accounter_;
    std::vector<net::Listener> listeners_;

    void Start();

    void Stop();
  };

  void Stop();

  void InitPortInfo(
      PortInfo& info, const ServerConfig& config,
      const net::ListenerConfig& listener_config,
      const components::ComponentContext& component_context, PortType port_type) const;

  void StartPortInfos();

  PortInfo main_port_info_, monitor_port_info_, tls_port_info_;
  std::unique_ptr<net::TlsSettings> tls_settings_;
  std::atomic<size_t> handlers_count_{0};

  mutable std::shared_timed_mutex stat_mutex_;
  bool is_stopping_{false};

  std::atomic<bool> started_{false};
  std::atomic<bool> has_requests_view_watchers_{false};
};

void ServerImpl::PortInfo::Stop() {
  static constexpr size_t kMaxSleepMs = 50;
  LOG_TRACE() << "Stopping listeners";
  listeners_.clear();
  LOG_TRACE() << "Stopped listeners";

  if (endpoint_info_) {
    // Connections are closing asynchronously.
    // So we need to wait until they close.
    for (size_t sleep_ms = 1; endpoint_info_->connection_count > 0;
         sleep_ms = std::min(sleep_ms * 2, kMaxSleepMs)) {
      LOG_DEBUG()
          << "sleeping while connections are still closing... connection_count="
          << endpoint_info_->connection_count << ", "
          << endpoint_info_->GetDescription();
      engine::SleepFor(std::chrono::milliseconds(sleep_ms));
    }
  }

  LOG_TRACE() << "Stopping request handlers";
  request_handler_.reset();
  LOG_TRACE() << "Stopped request handlers";
}

void ServerImpl::PortInfo::Start() {
  UASSERT(request_handler_);
  request_handler_->DisableAddHandler();
  for (auto& listener : listeners_) {
    listener.Start();
  }
}

ServerImpl::ServerImpl(ServerConfig config,
                       const components::ComponentContext& component_context)
    : config_(std::move(config)),
      requests_view_(std::make_unique<RequestsView>()) {
  LOG_INFO() << "Creating server";

  InitPortInfo(main_port_info_, config_, config_.listener, component_context,
               PortType::kMain);
  if (config_.max_response_size_in_flight) {
    main_port_info_.data_accounter_.SetMaxLevel(
        *config_.max_response_size_in_flight);
  }
  if (config_.monitor_listener) {
    InitPortInfo(monitor_port_info_, config_, *config_.monitor_listener,
                 component_context, PortType::kMonitor);
  }
  if (config_.tls_listener) {
    std::cout << "start tls listener on port " << config_.tls_listener->port <<std::endl;;
    auto* secdist_component =
        component_context.FindComponentOptional<components::Secdist>();
    tls_settings_ = std::make_unique<net::TlsSettings>(
        secdist_component->Get().Get<net::TlsSettings>());
    if (tls_settings_){
      std::cout << "tsl settings loaded" << std::endl;
    } else {
      std::cout << "tls settings NOT LOADED" << std::endl;
    }

    InitPortInfo(tls_port_info_, config_, *config_.tls_listener,
                 component_context, PortType::kSecure);
  }

  LOG_INFO() << "Server is created";
}

ServerImpl::~ServerImpl() { Stop(); }

void ServerImpl::Stop() {
  {
    std::unique_lock<std::shared_timed_mutex> lock(stat_mutex_);
    if (is_stopping_) return;
    is_stopping_ = true;
  }

  LOG_INFO() << "Stopping server";
  main_port_info_.Stop();
  monitor_port_info_.Stop();
  tls_port_info_.Stop();
  LOG_INFO() << "Stopped server";
}

void ServerImpl::InitPortInfo(
    PortInfo& info, const ServerConfig& config,
    const net::ListenerConfig& listener_config,
    const components::ComponentContext& component_context, PortType port_type) const {
  LOG_INFO() << "Creating listener (" << kPortTypeNamesMap.TryFind(port_type)<< ")";

  engine::TaskProcessor& task_processor =
      component_context.GetTaskProcessor(listener_config.task_processor);

  if (port_type != PortType::kSecure) {
    info.request_handler_ = std::make_shared<http::HttpRequestHandler>(
        component_context, config.logger_access, config.logger_access_tskv,
        port_type == PortType::kMonitor, config.server_name);
  } else {
    info.request_handler_ = main_port_info_.request_handler_;
  }

  info.endpoint_info_ = std::make_shared<net::EndpointInfo>(
      listener_config, *info.request_handler_);

  const auto& event_thread_pool = task_processor.EventThreadPool();
  size_t listener_shards = listener_config.shards ? *listener_config.shards
                                                  : event_thread_pool.GetSize();

  while (listener_shards--) {
    if (port_type == PortType::kSecure){
      std::cout << "Try start listener"<< std::endl;
      info.listeners_.emplace_back(info.endpoint_info_, task_processor,
                                   info.data_accounter_, tls_settings_.get());
      std::cout << "Listener started. May be successfull"<< std::endl;
    } else{
      info.listeners_.emplace_back(info.endpoint_info_, task_processor,
                                   info.data_accounter_, nullptr);
    }
  }
}

void ServerImpl::StartPortInfos() {
  UASSERT(main_port_info_.request_handler_);

  if (has_requests_view_watchers_.load()) {
    auto queue = requests_view_->GetQueue();
    requests_view_->StartBackgroundWorker();
    auto hook = [queue](std::shared_ptr<request::RequestBase> request) {
      queue->enqueue(request);
    };
    main_port_info_.request_handler_->SetNewRequestHook(hook);
    if (monitor_port_info_.request_handler_) {
      monitor_port_info_.request_handler_->SetNewRequestHook(hook);
    }
  }

  main_port_info_.Start();
  if (monitor_port_info_.request_handler_) {
    monitor_port_info_.Start();
  } else {
    LOG_WARNING() << "No 'listener-monitor' in 'server' component";
  }
  
  if (tls_port_info_.request_handler_){
    tls_port_info_.Start();
  } else {
    std::cout << "TLS port not started" <<std::endl;
  }

  started_.store(true);
}

Server::Server(ServerConfig config,
               const components::ComponentContext& component_context)
    : pimpl(
          std::make_unique<ServerImpl>(std::move(config), component_context)) {}

Server::~Server() = default;

const ServerConfig& Server::GetConfig() const { return pimpl->config_; }

void Server::WriteMonitorData(utils::statistics::Writer& writer) const {
  auto server_stats = pimpl->GetServerStats();
  if (auto json_conn_stats = writer["connections"]) {
    json_conn_stats["active"] = server_stats.active_connections;
    json_conn_stats["opened"] = server_stats.connections_created;
    json_conn_stats["closed"] = server_stats.connections_closed;
  }

  if (auto json_request_stats = writer["requests"]) {
    json_request_stats["active"] = server_stats.active_request_count;
    json_request_stats["avg-lifetime-ms"] =
        pimpl->main_port_info_.data_accounter_.GetAvgRequestTime().count();
    json_request_stats["processed"] = server_stats.requests_processed_count;
    json_request_stats["parsing"] =
        server_stats.parser_stats.parsing_request_count;
  }
}

void Server::WriteTotalHandlerStatistics(
    utils::statistics::Writer& writer) const {
  const auto& handlers =
      pimpl->main_port_info_.request_handler_->GetHandlerInfoIndex()
          .GetHandlers();

  handlers::HttpHandlerStatisticsSnapshot total;
  for (const auto handler_ptr : handlers) {
    const auto& statistics = handler_ptr->GetHandlerStatistics().GetTotal();
    total.Add(handlers::HttpHandlerStatisticsSnapshot{statistics});
  }

  writer = total;
}

net::Stats Server::GetServerStats() const { return pimpl->GetServerStats(); }

void Server::AddHandler(const handlers::HttpHandlerBase& handler,
                        engine::TaskProcessor& task_processor) {
  if (handler.IsMonitor() && !pimpl->monitor_port_info_.request_handler_) {
    throw std::logic_error(
        "Attempt to register a handler for 'listener-monitor' that was not "
        "configured in 'server' section of the component config");
  }

  if (handler.IsMonitor()) {
    UINVARIANT(pimpl->monitor_port_info_.request_handler_,
               "Attempt to register monitor handler while the server has no "
               "'listener-monitor'");
    pimpl->monitor_port_info_.request_handler_->AddHandler(handler,
                                                           task_processor);
  } else {
    UASSERT(pimpl->main_port_info_.request_handler_);
    pimpl->main_port_info_.request_handler_->AddHandler(handler,
                                                        task_processor);
  }

  if (!handler.IsMonitor()) {
    if (handler.GetConfig().throttling_enabled) {
      pimpl->handlers_count_++;
    }
  }
}

size_t Server::GetRegisteredHandlersCount() const {
  return pimpl->handlers_count_.load();
}

const http::HttpRequestHandler& Server::GetHttpRequestHandler(
    bool is_monitor) const {
  if (is_monitor) {
    UASSERT(pimpl->monitor_port_info_.request_handler_);
    return *pimpl->monitor_port_info_.request_handler_;
  }

  UASSERT(pimpl->main_port_info_.request_handler_);
  return *pimpl->main_port_info_.request_handler_;
}

void Server::Start() {
  LOG_INFO() << "Starting server";
  pimpl->StartPortInfos();
  LOG_INFO() << "Server is started";
}

void Server::Stop() { pimpl->Stop(); }

RequestsView& Server::GetRequestsView() {
  UASSERT(!pimpl->started_ || pimpl->has_requests_view_watchers_.load());

  pimpl->has_requests_view_watchers_.store(true);
  return *pimpl->requests_view_;
}

void Server::SetRpsRatelimit(std::optional<size_t> rps) {
  pimpl->main_port_info_.request_handler_->SetRpsRatelimit(rps);
}

void Server::SetRpsRatelimitStatusCode(http::HttpStatus status_code) {
  pimpl->main_port_info_.request_handler_->SetRpsRatelimitStatusCode(
      status_code);
}

net::Stats ServerImpl::GetServerStats() const {
  net::Stats summary;

  std::shared_lock<std::shared_timed_mutex> lock(stat_mutex_);
  if (is_stopping_) return summary;
  for (const auto& listener : main_port_info_.listeners_) {
    summary += listener.GetStats();
  }

  return summary;
}

}  // namespace server

USERVER_NAMESPACE_END

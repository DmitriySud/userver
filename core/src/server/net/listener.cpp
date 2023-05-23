#include "listener.hpp"

#include <iostream>
#include <userver/engine/async.hpp>
#include <userver/logging/log.hpp>
#include "server/net/tls_settings.hpp"

USERVER_NAMESPACE_BEGIN

namespace server::net {

Listener::Listener(std::shared_ptr<EndpointInfo> endpoint_info,
                   engine::TaskProcessor& task_processor,
                   request::ResponseDataAccounter& data_accounter,
                   const TlsSettings* tls_settings)
    : task_processor_(&task_processor),
      endpoint_info_(std::move(endpoint_info)),
      data_accounter_(&data_accounter), 
      tls_settings_(tls_settings){
        std::cout << "Init listener wit tls_settings" << tls_settings_<< std::endl;
      }

void Listener::Start() {
  std::cout << "Start listener " <<std::endl;
  if (tls_settings_){
    std::cout << "Start Tls listener impl" << std::endl;
  }
  impl_ = std::make_unique<ListenerImpl>(*task_processor_, endpoint_info_,
                                         *data_accounter_, tls_settings_);
}

Listener::~Listener() {
  if (!impl_) return;

  LOG_TRACE() << "Destroying listener";
  impl_.reset();
  LOG_TRACE() << "Destroyed listener";
}

Stats Listener::GetStats() const {
  if (impl_) return impl_->GetStats();
  return Stats{};
}

}  // namespace server::net

USERVER_NAMESPACE_END

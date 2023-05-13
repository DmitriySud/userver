#pragma once

#include <memory>

#include <userver/engine/task/task_processor_fwd.hpp>

#include "endpoint_info.hpp"
#include "listener_impl.hpp"
#include "server/net/listener.hpp"
#include "stats.hpp"

USERVER_NAMESPACE_BEGIN

namespace server::net {

class NonSecureListener : public Listener {
 public:
  NonSecureListener(std::shared_ptr<EndpointInfo> endpoint_info,
                    engine::TaskProcessor& task_processor,
                    request::ResponseDataAccounter& data_accounter)
      : Listener(endpoint_info, task_processor, data_accounter) {}

  ~NonSecureListener();

  NonSecureListener(const NonSecureListener&) = delete;
  NonSecureListener(NonSecureListener&&) noexcept = default;
  NonSecureListener& operator=(const NonSecureListener&) = delete;
  NonSecureListener& operator=(NonSecureListener&&) = default;

  void Start() override {
    impl_ = std::make_unique<ListenerImpl>(*task_processor_, endpoint_info_,
                                         *data_accounter_);
  }
};

}  // namespace server::net

USERVER_NAMESPACE_END


#pragma once

#include <server/net/listener.hpp>
#include <userver/crypto/private_key.hpp>
#include <userver/crypto/certificate.hpp>
#include "server/net/tls_settings.hpp"

USERVER_NAMESPACE_BEGIN

namespace server::net{

class TlsListener final : public Listener {
public:
 TlsListener(std::shared_ptr<EndpointInfo> endpoint_info,
             engine::TaskProcessor& task_processor,
             request::ResponseDataAccounter& data_accounter,
             const TlsSettings& settings)
     : Listener(endpoint_info, task_processor, data_accounter),
       settings_(settings) {}

 TlsListener(const TlsListener&) = delete;
 TlsListener(TlsListener&&) noexcept = delete;
 TlsListener& operator=(const TlsListener&) = delete;
 TlsListener& operator=(TlsListener&&) = delete;

 void Start() override {
   // TBD
 }

private:
  const TlsSettings& settings_;
};

}  // namespace server::net

USERVER_NAMESPACE_END

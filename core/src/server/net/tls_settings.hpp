#pragma once

#include <memory>
#include <userver/formats/json/value.hpp>
#include "userver/crypto/certificate.hpp"
#include "userver/crypto/private_key.hpp"

USERVER_NAMESPACE_BEGIN

namespace server::net {

class TlsSettings {
 public:
  TlsSettings(const formats::json::Value& doc);
private:
  std::optional<const crypto::PrivateKey> key_;
  std::optional<const crypto::Certificate> cert_;
};


}  // namespace server::net

USERVER_NAMESPACE_END


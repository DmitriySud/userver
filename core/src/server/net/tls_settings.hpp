#pragma once

#include <memory>
#include <userver/formats/json/value.hpp>
#include "userver/crypto/certificate.hpp"
#include "userver/crypto/private_key.hpp"
#include "userver/utils/assert.hpp"

USERVER_NAMESPACE_BEGIN

namespace server::net {

class TlsSettings final {
public:
  TlsSettings(const formats::json::Value& doc);

  const crypto::PrivateKey& GetKey() const {
    UASSERT(key_);
    return key_.value();
  }

  const crypto::Certificate& GetCertificate() const {
    UASSERT(cert_);
    return cert_.value();
  }
private:
  std::optional<const crypto::PrivateKey> key_;
  std::optional<const crypto::Certificate> cert_;
};


}  // namespace server::net

USERVER_NAMESPACE_END


#include "tls_settings.hpp"

#include <memory>
#include <string_view>
#include "userver/crypto/certificate.hpp"
#include "userver/crypto/private_key.hpp"
#include "userver/logging/log.hpp"

USERVER_NAMESPACE_BEGIN

namespace server::net {

namespace {

constexpr std::string_view kSectionName = "tls_settings";

}

TlsSettings::TlsSettings(const formats::json::Value& doc){
  const formats::json::Value& settings = doc[kSectionName];
  if (settings.IsMissing()){
    LOG_WARNING() << "There is no tls settings section it secdist";
    return;
  }
  
  key_path_ = settings["private_key"].As<std::string>();
  cert_path_ = settings["certificate"].As<std::string>();

  key_.emplace(crypto::PrivateKey::LoadFromString(
        fs::blocking::ReadFileContents(key_path_)
  ));
  cert_.emplace(crypto::Certificate::LoadFromString(
        fs::blocking::ReadFileContents(cert_path_)
  ));
}

}  // namespace server::net

USERVER_NAMESPACE_END



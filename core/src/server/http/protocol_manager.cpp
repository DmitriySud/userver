#include "protocol_manager.hpp"

#include <userver/http/common_headers.hpp>

namespace {

bool findStringIC(const std::string& strHaystack,
                  const std::string& strNeedle) {
  std::basic_string<char>::const_iterator it =
      std::search(strHaystack.begin(), strHaystack.end(), strNeedle.begin(),
                  strNeedle.end(), [](unsigned char ch1, unsigned char ch2) {
                    return std::toupper(ch1) == std::toupper(ch2);
                  });
  return (it != strHaystack.end());
}

}  // namespace

USERVER_NAMESPACE_BEGIN

namespace server::http {

ProtocolManager::ProtocolManager(http::HttpRequestParser& http_parser): 
  http1_handler_(http_parser){
}

bool ProtocolManager::Parse(const char* data, size_t size) {
  if (http_protocol_ == HttpProtocol::kHttp) {
    bool ret = http1_handler_->Parse(data, size);
    return ret;
  } else {
    // TODO: Add http2 parsing
    // return http2_handler_->Parse(data, size);
    return false;
  }
}

void ProtocolManager::SendResponse(engine::io::Socket& socket,
                                   request::ResponseBase& response) {
  if (http_protocol_ == HttpProtocol::kHttp) {
    http1_handler_->SendResponse(socket, response);
  } else {
    // TODO: Add send response when HTTP2Handler will be implemented
    // http2_handler_->SendResponse(socket, response);
  }
}

void ProtocolManager::CheckUpgrade(const http::HttpRequestImpl& http_request) {
  if (http_request.HasUpgradeHeaders() && CheckUpgradeHeaders(http_request)) {
    // TODO: delete (avoiding make static method hint)
    if (http_protocol_ == HttpProtocol::kHttp) {
    }

    // TODO: SwitchProtocol when HTTP2Handler will be implemented
    // UpgradeToHttp2(http_request.GetHeader(USERVER_NAMESPACE::http::headers::kHttp2Settings));
  }
}

bool ProtocolManager::CheckUpgradeHeaders(
    const http::HttpRequestImpl& http_request) {
  if (!findStringIC(
          http_request.GetHeader(USERVER_NAMESPACE::http::headers::kConnection),
          "upgrade")) {
    return false;
  }

  if (!findStringIC(
          http_request.GetHeader(USERVER_NAMESPACE::http::headers::kUpgrade),
          "web-sockets")) {
    return false;
  }

  return http_request.HasHeader(
      USERVER_NAMESPACE::http::headers::kSecWebSocketsKey);
}

void ProtocolManager::UpgradeToWebSockets(const std::string& settings) {
  http_protocol_ = HttpProtocol::kWebSockets;

  // TODO: delete (Unused)
  if (settings.empty()) {
  }
  // TODO: create http2_handler_ with settings
  // http2_handler_ = HTTP2Handler(settings, upgrade_data);
  // http2_handler_.SetNeedUpgrade(true);
}

}  // namespace server::http

USERVER_NAMESPACE_END

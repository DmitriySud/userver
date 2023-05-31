#pragma once

#include "protocol_handler.hpp"

#include <server/http/http_protocol_handler.hpp>
#include <server/http/http_request_parser.hpp>
#include <userver/engine/io/socket.hpp>
#include <userver/server/request/response_base.hpp>

USERVER_NAMESPACE_BEGIN

namespace server::http {

enum class HttpProtocol {
  kHttp = 0,
  kWebSockets = 2,
};

class ProtocolManager final : ProtocolHandler {
 public:
  ProtocolManager(http::HttpRequestParser& http_parser);

  bool Parse(const char* data, size_t size) override;
  void SendResponse(engine::io::Socket& socket,
                    request::ResponseBase& response) override ;

  void UpgradeToWebSockets(const std::string& settings);

  static bool CheckUpgradeHeaders(const http::HttpRequestImpl& http_request);
  void CheckUpgrade(const http::HttpRequestImpl& http_request);

 private:

  HttpProtocol http_protocol_{HttpProtocol::kHttp};

  std::optional<HttpProtocolHandler> http1_handler_;
};

}  // namespace server::http

USERVER_NAMESPACE_END


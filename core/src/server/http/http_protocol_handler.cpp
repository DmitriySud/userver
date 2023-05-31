#include "http_protocol_handler.hpp"

USERVER_NAMESPACE_BEGIN

namespace server::http {

HttpProtocolHandler::HttpProtocolHandler(http::HttpRequestParser& http_parser)
    : http1_parser_(http_parser) {}

bool HttpProtocolHandler::Parse(const char* data, size_t size) {
  return http1_parser_.Parse(data, size);
}

void HttpProtocolHandler::SendResponse(engine::io::Socket& socket,
                                request::ResponseBase& response) const {
  response.SendResponse(socket);
}

}  // namespace server::http

USERVER_NAMESPACE_END

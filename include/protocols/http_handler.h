#ifndef LOAD_BALANCER_HTTP_HANDLER_H
#define LOAD_BALANCER_HTTP_HANDLER_H

#include "protocol_handler.h"

#include <string>

namespace load_balancer {
namespace protocols {

// Handles HTTP/HTTPS traffic forwarding.
//
// This class extends ProtocolHandler to specifically manage HTTP and HTTPS
// client connections, including TLS handshakes and data proxying.
class HttpHandler : public ProtocolHandler {
 public:
  HttpHandler(int client_socket, std::shared_ptr<core::Router> router);
  ~HttpHandler() override;

  // Forwards HTTP/HTTPS traffic between client and backend.
  //
  // This method performs TLS handshakes on both client and backend sides,
  // then proxies data bidirectionally.
  void Forward() override;

 private:
  // Reads an incoming HTTP request from the client socket.
  std::string ReadHttpRequest();

  // Forwards an HTTP request to the selected backend server.
  void ForwardHttpRequest(const std::string& request, int backend_socket);
};

}  // namespace protocols
}  // namespace load_balancer

#endif  // LOAD_BALANCER_HTTP_HANDLER_H

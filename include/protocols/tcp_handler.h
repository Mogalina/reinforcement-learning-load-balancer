#ifndef LOAD_BALANCER_TCP_HANDLER_H
#define LOAD_BALANCER_TCP_HANDLER_H

#include "protocol_handler.h"

namespace load_balancer {
namespace protocols {

// Handles raw TCP traffic forwarding.
// This class extends ProtocolHandler to manage direct TCP client connections,
// including TLS handshakes and bidirectional data proxying.
class TcpHandler : public ProtocolHandler {
 public:
  TcpHandler(int client_socket, std::shared_ptr<core::Router> router);
  ~TcpHandler() override;

  // Forwards raw TCP traffic between client and backend.
  // This method establishes a connection to a backend, performs TLS handshakes,
  // and then proxies data bidirectionally.
  void Forward() override;
};

}  // namespace protocols
}  // namespace load_balancer

#endif  // LOAD_BALANCER_TCP_HANDLER_H

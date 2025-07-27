#ifndef LOAD_BALANCER_PROTOCOL_HANDLER_H
#define LOAD_BALANCER_PROTOCOL_HANDLER_H

#include "core/router.h"

#include <openssl/ssl.h>

namespace load_balancer {
namespace protocols {

// Abstract base class for handling different network protocols.
// Defines the interface for protocol-specific handlers responsible for
// forwarding client traffic to backend servers.
class ProtocolHandler {
 public:
  virtual ~ProtocolHandler() = default;

  // Derived classes must implement this to define how client traffic is handled
  // and forwarded according to the specific protocol.
  virtual void Forward() = 0;

 protected:
  ProtocolHandler(int client_socket, std::shared_ptr<core::Router> router)
      : client_socket_(client_socket), router_(std::move(router)) {}

  // Proxies data between two SSL connections.
  void Proxy(SSL* from, SSL* to);

  // File descriptor for the client's socket.
  int client_socket_;
  // Shared pointer to the Router for backend selection.
  std::shared_ptr<core::Router> router_;
};

}  // namespace protocols
}  // namespace load_balancer

#endif  // LOAD_BALANCER_PROTOCOL_HANDLER_H

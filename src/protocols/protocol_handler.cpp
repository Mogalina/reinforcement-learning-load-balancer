#include "protocols/protocol_handler.h"

namespace load_balancer {
namespace protocols {

void ProtocolHandler::Proxy(SSL* from, SSL* to) {
  constexpr size_t BUFFER_SIZE = 4096;
  char buffer[BUFFER_SIZE];

  while (true) {
    int bytes = SSL_read(from, buffer, BUFFER_SIZE);
    if (bytes <= 0) break;
    if (SSL_write(to, buffer, bytes) <= 0) break;
  }
}

}  // namespace protocols
}  // namespace load_balancer

#include "protocols/http_handler.h"
#include "utils/tls_utils.h"

#include <openssl/err.h>
#include <spdlog/spdlog.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <thread>

namespace load_balancer {
namespace protocols {

HttpHandler::HttpHandler(int client_socket,
                         std::shared_ptr<core::Router> router)
    : ProtocolHandler(client_socket, std::move(router)) {}

HttpHandler::~HttpHandler() = default;

void HttpHandler::Forward() {
  // Select a backend server to forward the load.
  auto backend = router_->PickBackendServer();
  if (!backend) {
    spdlog::error("No backend available for HTTP forwarding.");
    return;
  }

  // Create a socket for the connection to the backend server.
  int backend_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (backend_socket < 0) {
    spdlog::error("Failed to create backend socket: {}", strerror(errno));
    return;
  }

  // Configure the backend server address structure.
  sockaddr_in backend_addr{};
  backend_addr.sin_family = AF_INET;
  backend_addr.sin_port = htons(backend->Port());
  inet_pton(AF_INET, backend->Ip().c_str(), &backend_addr.sin_addr);

  // Connect to the backend server.
  if (connect(backend_socket, reinterpret_cast<sockaddr*>(&backend_addr),
              sizeof(backend_addr)) < 0) {
    spdlog::error("Failed to connect to backend {}:{} - {}",
                  backend->Ip(), backend->Port(), strerror(errno));
    close(backend_socket);
    return;
  }

  // --- TLS Handshake with Client (Load Balancer acts as Server) ---
  SSL_CTX* client_ctx = utils::TlsUtils::CreateContext(true);
  utils::TlsUtils::ConfigureContext(client_ctx, "cert.pem", "key.pem");

  SSL* ssl_client = SSL_new(client_ctx);
  SSL_set_fd(ssl_client, client_socket_);
  // Perform TLS handshake with client.
  if (SSL_accept(ssl_client) <= 0) {
    spdlog::error("TLS handshake with client failed");
    ERR_print_errors_fp(stderr);
    SSL_free(ssl_client);
    SSL_CTX_free(client_ctx);
    return;
  }

  // --- TLS Handshake with Backend (Load Balancer acts as Client) ---
  SSL_CTX* backend_ctx = utils::TlsUtils::CreateContext(false);
  SSL* ssl_backend = SSL_new(backend_ctx);
  SSL_set_fd(ssl_backend, backend_socket);
  // Perform TLS handshake with backend.
  if (SSL_connect(ssl_backend) <= 0) {
    spdlog::error("TLS handshake with backend failed");
    ERR_print_errors_fp(stderr);
    SSL_free(ssl_backend);
    SSL_CTX_free(backend_ctx);
    return;
  }

  // -- Bidirectional Data Forwarding --
  std::thread client_to_backend([=, this]() {
    // Proxy data from client to backend.
    Proxy(ssl_client, ssl_backend);
  });
  std::thread backend_to_client([=, this]() {
    // Proxy data from backend to client.
    Proxy(ssl_backend, ssl_client);
  });

  // Wait for both proxying threads to complete.
  client_to_backend.join();
  backend_to_client.join();

  // --- Cleanup SSL/TLS Resources ---
  SSL_shutdown(ssl_client);
  SSL_free(ssl_client);
  SSL_CTX_free(client_ctx);

  SSL_shutdown(ssl_backend);
  SSL_free(ssl_backend);
  SSL_CTX_free(backend_ctx);

  // Close the backend socket.
  close(backend_socket);
}

std::string HttpHandler::ReadHttpRequest() {
  constexpr size_t BUFFER_SIZE = 4096;
  char buffer[BUFFER_SIZE];
  std::string request;

  // Read from the client socket until no more bytes or end of request.
  while (true) {
    ssize_t bytes = read(client_socket_, buffer, BUFFER_SIZE);
    if (bytes <= 0) break;

    request.append(buffer, bytes);

    // Check if the end of the HTTP headers has been reached.
    if (request.find("\r\n\r\n") != std::string::npos) break;
  }

  return request;
}

void HttpHandler::ForwardHttpRequest(const std::string& request,
                                     int backend_socket) {
  // Send the entire request string to the backend socket.
  ssize_t bytes_sent = send(backend_socket, request.c_str(), request.size(), 0);
  if (bytes_sent < 0) {
    spdlog::error("Failed to send request to backend: {}", strerror(errno));
  }
}

}  // namespace protocols
}  // namespace load_balancer

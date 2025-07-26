#include "core/server.h"
#include "protocols/tcp_handler.h"
#include "spdlog/spdlog.h"

#include <arpa/inet.h>
#include <csignal>
#include <unistd.h>

namespace load_balancer {
namespace core {

// Maximum number of pending connections allowed in the socket's listen queue.
constexpr int CONNECTIONS_QUEUE_SIZE = 100;

Server::Server(int port, std::shared_ptr<Router> router)
    : port_(port), server_socket_(-1), running_(false),
      router_(std::move(router)) {
  spdlog::debug("Server created on port {}", port_);
}

Server::~Server() {
  // Ensure resources are released.
  Stop();
}

void Server::Start() {
  // Create an IPv4 TCP socket stream.
  server_socket_ = socket(AF_INET, SOCK_STREAM, 0);
  if (server_socket_ < 0) {
    spdlog::error("Failed to create socket: {}", strerror(errno));
    return;
  }

  sockaddr_in server_addr{};
  // IPv4.
  server_addr.sin_family = AF_INET;
  // Listen to all interfaces.
  server_addr.sin_addr.s_addr = INADDR_ANY;
  // Convert port to network byte order.
  server_addr.sin_port = htons(port_);

  // Allow reuse of the address after the server shuts down.
  int opt = 1;
  if (setsockopt(server_socket_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) <
      0) {
    spdlog::error("Call to 'setsockopt' failed: {}",
                  strerror(errno));
    close(server_socket_);
    return;
  }

  // Bind the socket to the address and port.
  if (bind(server_socket_, reinterpret_cast<sockaddr*>(&server_addr),
           sizeof(server_addr)) < 0) {
    spdlog::error("Bind failed: {}", strerror(errno));
    close(server_socket_);
    return;
  }

  // Start listening for incoming connections.
  if (listen(server_socket_, CONNECTIONS_QUEUE_SIZE) < 0) {
    spdlog::error("Listen failed: {}", strerror(errno));
    close(server_socket_);
    return;
  }

  running_ = true;
  spdlog::info("Server listening on port {}", port_);

  // Start accepting connections in a blocking loop.
  AcceptConnections();
}

void Server::Stop() {
  if (!running_) return;

  // Close the main server socket.
  running_ = false;
  close(server_socket_);

  // Gracefully join all worker threads.
  for (auto &thread: worker_threads_)
    if (thread.joinable()) thread.join();

  spdlog::info("Server shutdown complete.");
}

void Server::AcceptConnections() {
  while (running_) {
    sockaddr_in client_addr{};
    socklen_t client_len = sizeof(client_addr);

    // Accept a new client connection.
    int client_socket = accept(server_socket_,
                               reinterpret_cast<sockaddr*>(&client_addr),
                               &client_len);
    if (client_socket < 0) {
      if (running_)
        spdlog::warn("Accept client connection failed: {}",
                     strerror(errno));
      continue;
    }

    spdlog::info("New client connected from {}:{}",
                 inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    // Start a new thread to handle this client.
    worker_threads_.emplace_back([this, client_socket]() {
      HandleClient(client_socket);
    });
  }
}

void Server::HandleClient(int client_socket) {
  // Encapsulates protocol logic for this client.
  protocols::TcpHandler handler(client_socket, router_);

  // Forward traffic between client and selected backend server.
  handler.Forward();

  // Cleanup.
  close(client_socket);
  spdlog::debug("CLosed client socket: {}", client_socket);
}

}  // namespace core
}  // namespace load_balancer

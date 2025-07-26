#ifndef LOAD_BALANCER_SERVER_H
#define LOAD_BALANCER_SERVER_H

#include "router.h"

#include <string>
#include <thread>
#include <vector>
#include <atomic>
#include <memory>
#include <netinet/in.h>

namespace load_balancer {
namespace core {

// The Server class manages the core functionality of the load balancer.
// This class is responsible for initializing a TCP server, listening for
// incoming client connections, and dispatching these connections to
// individual handler threads. It integrates with a Router to determine
// which backend server should handle the client's requests.
class Server {
 public:
  Server(int port, std::shared_ptr<Router> router);
  ~Server();

  // This class is not copyable or movable.
  Server(const Server& other) = delete;
  Server& operator=(const Server& other) = delete;
  Server(Server&& other) = delete;
  Server& operator=(Server&& other) = delete;

  // Start listening for incoming connections and handling clients.
  void Start();
  // Stop the server gracefully.
  void Stop();

 private:
  // Internal loop accepting incoming client connections.
  void AcceptConnections();
  // Handle an individual client connection.
  void HandleClient(int client_socket);

  // TCP port number to listen on.
  int port_;
  // File descriptor for server socket.
  int server_socket_;
  // Flag indicating if server is running.
  std::atomic<bool> running_;
  // Threads handling client connections.
  std::vector<std::thread> worker_threads_;
  // Shared router instance for backend selection.
  std::shared_ptr<Router> router_{};
};

}  // namespace core
}  // namespace load_balancer

#endif  // LOAD_BALANCER_SERVER_H

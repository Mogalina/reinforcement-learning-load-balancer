#ifndef LOAD_BALANCER_BACKEND_SERVER_H
#define LOAD_BALANCER_BACKEND_SERVER_H

#include <string>
#include <mutex>

namespace load_balancer {
namespace core {

// Represents a single backend server that can handle requests.
// Manages the state and properties of a backend server, including its health,
// active connections, and last check time.
class BackendServer {
 public:
  BackendServer(std::string ip, int port, int weight = 1);

  // This class is not copyable or movable.
  BackendServer(const BackendServer& other) = delete;
  BackendServer& operator=(const BackendServer& other) = delete;
  BackendServer(BackendServer&& other) = delete;
  BackendServer& operator=(BackendServer&& other) = delete;

  // Accessor methods for server properties.
  std::string Ip() const { return ip_; }
  int Port() const { return port_; }
  int Weight() const { return weight_; }
  bool IsHealthy() const { return healthy_; }
  int ActiveConnections() const { return active_connections_; }
  std::chrono::steady_clock::time_point
  LastChecked() const;

  // Mutator methods for server state.
  void SetHealthy(bool healthy) { healthy_ = healthy; }
  void IncrementConnections() { active_connections_++; }
  void DecrementConnections();
  void UpdateLastChecked();

 private:
  // The IP address of the backend server.
  std::string ip_;
  // The port number of the backend server.
  int port_;
  // The weight for load balancing.
  int weight_;

  // True if the server is considered healthy.
  std::atomic<bool> healthy_;
  // Number of active connections to this server.
  std::atomic<int> active_connections_;
  // Timestamp of the last health check.
  std::chrono::steady_clock::time_point last_checked_;
  // Mutex to protect access to 'last_checked_' field.
  mutable std::mutex time_mutex_;
};

}  // namespace core
}  // namespace load_balancer

#endif  // LOAD_BALANCER_BACKEND_SERVER_H

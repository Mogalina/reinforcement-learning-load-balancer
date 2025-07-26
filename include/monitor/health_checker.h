#ifndef LOAD_BALANCER_HEALTH_CHECKER_H
#define LOAD_BALANCER_HEALTH_CHECKER_H

#include "core/backend_server.h"

#include <vector>
#include <memory>
#include <thread>
#include <atomic>
#include <mutex>

namespace load_balancer {
namespace monitor {

// Periodically checks the health of registered backend servers.
// This class runs a background thread that attempts to connect to each backend
// server at a specified interval to determine its availability.
class HealthChecker {
 public:
  explicit HealthChecker(
      std::vector<std::shared_ptr<core::BackendServer>> backends,
      int interval_seconds = 10);
  ~HealthChecker();

  // Starts the health checking process in a new thread.
  void Start();
  // Stops the health checking thread gracefully.
  void Stop();

 private:
  // The main loop for periodic health checks. Runs in a separate thread and
  // calls CheckServerHealth for each backend.
  void CheckLoop();
  // Performs a single health check on a given backend server. Attempts to
  // establish a TCP connection to the server's IP and port.
  bool CheckServerHealth(const std::shared_ptr<core::BackendServer>& server);

  // The list of backend servers to monitor.
  std::vector<std::shared_ptr<core::BackendServer>> backends_;
  // The interval between health checks in seconds.
  int interval_seconds_;
  // Atomic flag to control the running state of the checker thread.
  std::atomic<bool> running_;
  // The thread that runs the health checking loop.
  std::thread checker_thread_;
  // Mutex for potential synchronization.
  std::mutex mutex_;
};

}  // namespace monitor
}  // namespace load_balancer

#endif  // LOAD_BALANCER_HEALTH_CHECKER_H

#include "monitor/health_checker.h"

#include <spdlog/spdlog.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

namespace load_balancer {
namespace monitor {

HealthChecker::HealthChecker(
    std::vector<std::shared_ptr<core::BackendServer>> backends,
    int interval_seconds)
    : backends_(std::move(backends)), interval_seconds_(interval_seconds),
      running_(false) {}

HealthChecker::~HealthChecker() {
  Stop();
}

void HealthChecker::Start() {
  if (running_) return;
  running_ = true;
  checker_thread_ = std::thread(&HealthChecker::CheckLoop, this);
}

void HealthChecker::Stop() {
  running_ = false;
  if (checker_thread_.joinable()) checker_thread_.join();
}

void HealthChecker::CheckLoop() {
  while (running_) {
    // Sleep for the specified interval before the next round of checks.
    std::this_thread::sleep_for(std::chrono::seconds(interval_seconds_));

    for (auto &server: backends_) {
      bool healthy = CheckServerHealth(server);
      server->SetHealthy(healthy);
      if (!healthy) {
        spdlog::warn("Backend server {}:{} is unhealthy.", server->Ip(),
                     server->Port());
      } else {
        spdlog::debug("Backend server {}:{} is healthy.", server->Ip(),
                      server->Port());
      }
    }
  }
}

bool HealthChecker::CheckServerHealth(
    const std::shared_ptr<core::BackendServer> &server) {
  // Create a TCP socket.
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
    spdlog::error("Failed to create health check socket for {}:{}",
                  server->Ip(), server->Port());
    return false;
  }

  sockaddr_in addr{};
  addr.sin_family = AF_INET;
  addr.sin_port = htons(server->Port());
  if (inet_pton(AF_INET, server->Ip().c_str(), &addr.sin_addr) <= 0) {
    spdlog::error("Invalid IP address for health check: {}", server->Ip());
    close(sock);
    return false;
  }

  // Attempt to connect to the backend server. A return value of 0 indicates
  // success.
  bool result = connect(sock, (sockaddr *) &addr, sizeof(addr)) == 0;
  close(sock);
  return result;
}

}  // namespace monitor
}  // namespace load_balancer

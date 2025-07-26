#include "core/backend_server.h"

#include <utility>

namespace load_balancer {
namespace core {

BackendServer::BackendServer(std::string ip, int port, int weight)
    : ip_(std::move(ip)), port_(port), weight_(weight),
      healthy_(true), active_connections_(0),
      last_checked_(std::chrono::steady_clock::now()) {}

std::chrono::steady_clock::time_point BackendServer::LastChecked() const {
  std::lock_guard<std::mutex> lock(time_mutex_);
  return last_checked_;
}

void BackendServer::DecrementConnections() {
  int curr = active_connections_;
  if (curr > 0) {
    active_connections_--;
  }
}

void BackendServer::UpdateLastChecked() {
  std::lock_guard<std::mutex> lock(time_mutex_);
  last_checked_ = std::chrono::steady_clock::now();
}

}  // namespace core
}  // namespace load_balancer

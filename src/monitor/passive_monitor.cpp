#include "monitor/passive_monitor.h"

#include <spdlog/spdlog.h>

namespace load_balancer {
namespace monitor {

void PassiveMonitor::RecordFailure(
    const std::shared_ptr<core::BackendServer>& backend) {
  std::lock_guard<std::mutex> lock(stats_mutex_);
  auto& stats = backend_stats_[backend->Ip()];
  stats.failure_count++;
  stats.last_failure_time = std::chrono::steady_clock::now();
  spdlog::warn("Recorded failure for backend {}: failure count = {}",
               backend->Ip(), stats.failure_count);
}

void PassiveMonitor::RecordSuccess(
    const std::shared_ptr<core::BackendServer>& backend) {
  std::lock_guard<std::mutex> lock(stats_mutex_);
  auto& stats = backend_stats_[backend->Ip()];
  stats.success_count++;

  // If enough consecutive successes, reset failure counts.
  if (stats.success_count >= kSuccessResetThreshold) {
    stats.failure_count = 0;
    stats.success_count = 0;
    spdlog::info(
        "Reset failure count for backend {} after consecutive successes",
        backend->Ip());
  }
}

bool PassiveMonitor::IsBackendSuspect(
    const std::shared_ptr<core::BackendServer>& backend) {
  std::lock_guard<std::mutex> lock(stats_mutex_);

  // Find the backend's statistics. If not found, it's not suspect.
  auto it = backend_stats_.find(backend->Ip());
  if (it == backend_stats_.end()) return false;

  // Get a reference to the statistics.
  const auto& stats = it->second;

  // Check if failure count has reached the threshold.
  if (stats.failure_count >= kFailureThreshold) {
    auto now = std::chrono::steady_clock::now();
    auto time_since_last_failure = now - stats.last_failure_time;
    // Check if it's within the quarantine period.
    if (time_since_last_failure < kQuarantineTime) {
      spdlog::debug("Backend {} is quarantined due to failure threshold",
                    backend->Ip());
      return true;
    }
  }
  return false;
}

}  // namespace monitor
}  // namespace load_balancer

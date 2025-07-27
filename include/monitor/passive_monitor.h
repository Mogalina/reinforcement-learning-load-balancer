#ifndef LOAD_BALANCER_PASSIVE_MONITOR_H
#define LOAD_BALANCER_PASSIVE_MONITOR_H

#include "core/backend_server.h"

#include <memory>
#include <unordered_map>
#include <mutex>
#include <chrono>

namespace load_balancer {
namespace monitor {

// Implements passive health monitoring for backend servers.
// This class tracks success and failure rates of backend servers based on
// observed traffic. It can mark servers as suspect after a certain number of
// failures and introduce a quarantine period.
class PassiveMonitor {
 public:
  PassiveMonitor() = default;

  // Records a connection failure for a given backend server.
  // Increments the failure count and updates the last failure time.
  void RecordFailure(const std::shared_ptr<core::BackendServer>& backend);

  // Records a successful connection for a given backend server.
  // Increments the success count and can reset failure counts if enough
  // consecutive successes occur.
  void RecordSuccess(const std::shared_ptr<core::BackendServer>& backend);

  // Checks if a backend server is currently considered suspect.
  // A server becomes suspect if its failure count exceeds a threshold and
  // it's within a quarantine period since the last failure.
  bool IsBackendSuspect(const std::shared_ptr<core::BackendServer>& backend);

 private:
  // Internal structure to store statistics for each backend server.
  struct BackendStats {
    // Number of consecutive failures.
    int failure_count = 0;
    // Number of consecutive successes.
    int success_count = 0;
    // Timestamp of the last recorded failure.
    std::chrono::steady_clock::time_point last_failure_time;
  };

  // Maps backend IP to its statistics.
  std::unordered_map<std::string, BackendStats> backend_stats_;
  // Mutex to protect 'backend_stats_' from concurrent access.
  std::mutex stats_mutex_;

  // Number of failures after which a backend becomes suspect.
  static constexpr int kFailureThreshold = 3;
  // Number of consecutive successes to reset failure count.
  static constexpr int kSuccessResetThreshold = 2;
  // Time a suspect backend remains quarantined.
  static constexpr std::chrono::seconds kQuarantineTime{30};
};

}  // namespace monitor
}  // namespace load_balancer

#endif  // LOAD_BALANCER_PASSIVE_MONITOR_H

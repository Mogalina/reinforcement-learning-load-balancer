#ifndef LOAD_BALANCER_MONITOR_METRICS_COLLECTOR_H_
#define LOAD_BALANCER_MONITOR_METRICS_COLLECTOR_H_

#include "core/backend_server.h"

#include <mutex>
#include <unordered_map>
#include <string>
#include <chrono>

namespace load_balancer {
namespace monitor {

// Structure to hold various metrics for a single backend.
struct Metrics {
  // Count of all requests.
  int total_requests = 0;
  // Count of successful requests.
  int total_successes = 0;
  // Count of failed requests.
  int total_failures = 0;
  // Sum of all recorded latencies in milliseconds.
  int total_latency_ms = 0;
  // Number of latency samples recorded.
  int latency_samples = 0;
  // Last recorded CPU usage percent.
  double cpu_usage_percent = 0.0;
  // Last recorded memory usage in MB.
  double memory_usage_mb = 0.0;
};

// Collects and provides performance metrics for backend servers.
// This class tracks various metrics such as request counts, success/failure
// rates, and latency for each backend server, ensuring thread-safe access.
class MetricsCollector {
 public:
  MetricsCollector() = default;

  // Record a request sent to a backend.
  void RecordRequest(const std::shared_ptr<core::BackendServer>& backend);

  // Record a successful response from a backend.
  void RecordSuccess(const std::shared_ptr<core::BackendServer>& backend);

  // Record a failed response from a backend.
  void RecordFailure(const std::shared_ptr<core::BackendServer>& backend);

  // Record latency (round-trip time) for a backend.
  void RecordLatency(const std::shared_ptr<core::BackendServer>& backend,
                     std::chrono::milliseconds latency);

  // Get total requests for a backend.
  int GetRequestCount(const std::shared_ptr<core::BackendServer>& backend);

  // Get number of successes.
  int GetSuccessCount(const std::shared_ptr<core::BackendServer>& backend);

  // Get number of failures.
  int GetFailureCount(const std::shared_ptr<core::BackendServer>& backend);

  // Get average latency in milliseconds.
  double GetAverageLatency(const std::shared_ptr<core::BackendServer>& backend);

  // Get CPU usage (last fetched).
  double GetCpuUsage(const std::shared_ptr<core::BackendServer>& backend);

  // Get memory usage (last fetched).
  double GetMemoryUsage(const std::shared_ptr<core::BackendServer>& backend);

  // Get collected metrics for all backends.
  const std::unordered_map<std::string, Metrics> MetricsMap();

 private:
  // Maps backend IP to its collected metrics.
  std::unordered_map<std::string, Metrics> metrics_map_;
  // Mutex to ensure thread-safe access to 'metrics_map_' field.
  std::mutex metrics_mutex_;
};

}  // namespace monitor
}  // namespace load_balancer

#endif  // LOAD_BALANCER_MONITOR_METRICS_COLLECTOR_H_

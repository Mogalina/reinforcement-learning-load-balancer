#include "metrics/metrics_collector.h"

namespace load_balancer {
namespace monitor {

void MetricsCollector::RecordRequest(
    const std::shared_ptr<core::BackendServer>& backend) {
  std::lock_guard<std::mutex> lock(metrics_mutex_);
  metrics_map_[backend->Ip()].total_requests++;
}

void MetricsCollector::RecordSuccess(
    const std::shared_ptr<core::BackendServer>& backend) {
  std::lock_guard<std::mutex> lock(metrics_mutex_);
  metrics_map_[backend->Ip()].total_successes++;
}

void MetricsCollector::RecordFailure(
    const std::shared_ptr<core::BackendServer>& backend) {
  std::lock_guard<std::mutex> lock(metrics_mutex_);
  metrics_map_[backend->Ip()].total_failures++;
}

void MetricsCollector::RecordLatency(
    const std::shared_ptr<core::BackendServer>& backend,
    std::chrono::milliseconds latency) {
  std::lock_guard<std::mutex> lock(metrics_mutex_);
  auto& metrics = metrics_map_[backend->Ip()];
  metrics.total_latency_ms += latency.count();
  metrics.latency_samples++;
}

int MetricsCollector::GetRequestCount(
    const std::shared_ptr<core::BackendServer>& backend) {
  std::lock_guard<std::mutex> lock(metrics_mutex_);
  return metrics_map_[backend->Ip()].total_requests;
}

int MetricsCollector::GetSuccessCount(
    const std::shared_ptr<core::BackendServer>& backend) {
  std::lock_guard<std::mutex> lock(metrics_mutex_);
  return metrics_map_[backend->Ip()].total_successes;
}

int MetricsCollector::GetFailureCount(
    const std::shared_ptr<core::BackendServer>& backend) {
  std::lock_guard<std::mutex> lock(metrics_mutex_);
  return metrics_map_[backend->Ip()].total_failures;
}

double MetricsCollector::GetAverageLatency(
    const std::shared_ptr<core::BackendServer>& backend) {
  std::lock_guard<std::mutex> lock(metrics_mutex_);
  const auto& metrics = metrics_map_[backend->Ip()];
  if (metrics.latency_samples == 0) return 0.0;
  return static_cast<double>(metrics.total_latency_ms) / metrics.latency_samples;
}

}  // namespace monitor
}  // namespace load_balancer

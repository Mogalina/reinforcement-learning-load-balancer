#ifndef LOAD_BALANCER_MONITOR_PROMETHEUS_EXPORTER_H_
#define LOAD_BALANCER_MONITOR_PROMETHEUS_EXPORTER_H_

#include "metrics/metrics_collector.h"

#include <memory>
#include <string>
#include <prometheus/exposer.h>
#include <prometheus/registry.h>
#include <prometheus/gauge.h>
#include <prometheus/counter.h>

namespace load_balancer {
namespace monitor {

// Exposes collected load balancer metrics in Prometheus format via an HTTP endpoint.
// This class leverages the 'prometheus-cpp' library to manage, register, and
// expose various metrics over a specified HTTP address.
class PrometheusExporter {
 public:
  explicit PrometheusExporter(
      const std::shared_ptr<MetricsCollector>& metrics_collector,
      const std::string& listen_addr);

  // Updates the Prometheus metrics with the latest values from the
  // MetricsCollector.
  // This method should be called periodically to push current metric values
  // into the Prometheus registry, which the Exposer then serves.
  void Export();

 private:
  // Helper function to register a counter family.
  prometheus::Family<prometheus::Counter>& RegisterCounterFamily(
      const std::string& name, const std::string& help);

  // Helper function to register a gauge family.
  prometheus::Family<prometheus::Gauge>& RegisterGaugeFamily(
      const std::string& name, const std::string& help);

  // Shared pointer to the MetricsCollector instance.
  std::shared_ptr<MetricsCollector> metrics_collector_;
  // The Prometheus registry where all metrics are registered.
  std::shared_ptr<prometheus::Registry> registry_;
  // The Prometheus HTTP server that exposes the metrics.
  std::unique_ptr<prometheus::Exposer> exposer_;

  // Metric family for total requests.
  prometheus::Family<prometheus::Counter>& request_counter_family_;
  // Metric family for total successes.
  prometheus::Family<prometheus::Counter>& success_counter_family_;
  // Metric family for total failures.
  prometheus::Family<prometheus::Counter>& failure_counter_family_;
  // Metric family for average latency.
  prometheus::Family<prometheus::Gauge>& latency_gauge_family_;
  // Metric family for CPU usage.
  prometheus::Family<prometheus::Gauge>& cpu_usage_gauge_family_;
  // Metric family for memory usage.
  prometheus::Family<prometheus::Gauge>& memory_usage_gauge_family_;
};

}  // namespace monitor
}  // namespace load_balancer

#endif  // LOAD_BALANCER_MONITOR_PROMETHEUS_EXPORTER_H_

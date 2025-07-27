#include "metrics/prometheus_exporter.h"

#include <prometheus/text_serializer.h>
#include <spdlog/spdlog.h>

namespace load_balancer {
namespace monitor {

prometheus::Family<prometheus::Counter>&
PrometheusExporter::RegisterCounterFamily(const std::string& name,
                                          const std::string& help) {
  return prometheus::BuildCounter()
      .Name(name)
      .Help(help)
      .Register(*registry_);
}

prometheus::Family<prometheus::Gauge>& PrometheusExporter::RegisterGaugeFamily(
    const std::string& name, const std::string& help) {
  return prometheus::BuildGauge()
      .Name(name)
      .Help(help)
      .Register(*registry_);
}

PrometheusExporter::PrometheusExporter(
    const std::shared_ptr<MetricsCollector>& metrics_collector,
    const std::string& listen_addr)
    : metrics_collector_(metrics_collector),
      registry_(std::make_shared<prometheus::Registry>()),
      exposer_(std::make_unique<prometheus::Exposer>(listen_addr)),
      // Initialize Counter metric families using the helper function.
      request_counter_family_(RegisterCounterFamily(
          "requests_total", "Total number of requests")),
      success_counter_family_(RegisterCounterFamily(
          "successes_total", "Total number of successes")),
      failure_counter_family_(RegisterCounterFamily(
          "failures_total", "Total number of failures")),
      // Initialize Gauge metric families using the helper function.
      latency_gauge_family_(RegisterGaugeFamily(
          "average_latency_ms", "Average latency in milliseconds")),
      cpu_usage_gauge_family_(RegisterGaugeFamily(
          "cpu_usage_percent", "CPU usage percent")),
      memory_usage_gauge_family_(RegisterGaugeFamily(
          "memory_usage_mb", "Memory usage in MB")) {
  // Register the entire metrics registry with the exposer.
  // This makes all registered metrics available via the HTTP endpoint.
  exposer_->RegisterCollectable(registry_);
  spdlog::info("Prometheus exporter initialized at {}", listen_addr);
}

void PrometheusExporter::Export() {
  // Get a consistent snapshot of all backend metrics.
  const auto backend_to_metrics_map = metrics_collector_->MetricsMap();
  for (const auto& [backend_ip, metrics] : backend_to_metrics_map) {
    // --- Update Counter Metrics ---
    auto& req_counter = request_counter_family_.Add({{"backend", backend_ip}});
    req_counter.Increment(metrics.total_requests);

    auto& success_counter = success_counter_family_.Add({{"backend", backend_ip}});
    success_counter.Increment();

    auto& fail_counter = failure_counter_family_.Add({{"backend", backend_ip}});
    fail_counter.Increment(metrics.total_failures);

    // --- Update Gauge Metrics ---
    auto& latency_gauge = latency_gauge_family_.Add({{"backend", backend_ip}});
    latency_gauge.Set(metrics.total_latency_ms);

    auto& cpu_gauge = cpu_usage_gauge_family_.Add({{"backend", backend_ip}});
    cpu_gauge.Set(metrics.cpu_usage_percent);

    auto& memory_gauge =
        memory_usage_gauge_family_.Add({{"backend", backend_ip}});
    memory_gauge.Set(metrics.memory_usage_mb);
  }
}

}  // namespace monitor
}  // namespace load_balancer

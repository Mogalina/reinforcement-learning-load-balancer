#include "metrics/prometheus_exporter.h"

#include <arpa/inet.h>
#include <unistd.h>
#include <sstream>
#include <spdlog/spdlog.h>

namespace load_balancer {
namespace monitor {

PrometheusExporter::PrometheusExporter(
    int port,
    std::shared_ptr<MetricsCollector> metrics_collector,
    const std::vector<std::shared_ptr<core::BackendServer>>& backends)
    : port_(port),
      server_socket_(-1),
      running_(false),
      metrics_collector_(std::move(metrics_collector)),
      backends_(backends) {}

PrometheusExporter::~PrometheusExporter() {
  Stop();
}

void PrometheusExporter::Start() {
  // Create an IPv4 TCP socket.
  server_socket_ = socket(AF_INET, SOCK_STREAM, 0);
  if (server_socket_ < 0) {
    spdlog::error("Prometheus exporter failed to create socket: {}",
                  strerror(errno));
    return;
  }

  // Configure server address structure.
  sockaddr_in addr{};
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(port_);

  // Set socket option to allow address reuse immediately after shutdown.
  int opt = 1;
  setsockopt(server_socket_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  // Bind the socket to the specified address and port.
  if (bind(server_socket_, reinterpret_cast<sockaddr*>(&addr),
           sizeof(addr)) < 0) {
    spdlog::error("Prometheus exporter bind failed: {}", strerror(errno));
    close(server_socket_);
    return;
  }

  // Start listening for incoming connections with a backlog of 10.
  if (listen(server_socket_, 10) < 0) {
    spdlog::error("Prometheus exporter listen failed: {}", strerror(errno));
    close(server_socket_);
    return;
  }

  running_ = true;
  // Launch a new thread to run the Serve method.
  server_thread_ = std::thread(&PrometheusExporter::Serve, this);
  spdlog::info("Prometheus exporter started on port {}", port_);
}

void PrometheusExporter::Stop() {
  if (!running_) return;
  running_ = false;
  close(server_socket_);
  if (server_thread_.joinable()) server_thread_.join();
  spdlog::info("Prometheus exporter stopped.");
}

void PrometheusExporter::Serve() {
  while (running_) {
    sockaddr_in client_addr{};
    socklen_t client_len = sizeof(client_addr);
    // Accept a new client connection. This call is blocking.
    int client_socket = accept(server_socket_,
                               reinterpret_cast<sockaddr*>(&client_addr),
                               &client_len);
    if (client_socket < 0) {
      if (running_) {
        spdlog::warn("Prometheus exporter failed to accept: {}",
                     strerror(errno));
      }
      continue;
    }
    HandleClient(client_socket);
    close(client_socket);
  }
}

void PrometheusExporter::HandleClient(int client_socket) {
  // Standard HTTP header for Prometheus metrics exposition.
  constexpr const char* response_header =
      "HTTP/1.1 200 OK\r\nContent-Type: text/plain; version=0.0.4\r\n\r\n";

  // Build the metrics string in Prometheus format.
  std::string metrics = BuildMetricsResponse();
  // Concatenate header and metrics.
  std::string full_response = response_header + metrics;

  // Send the complete HTTP response to the client.
  send(client_socket, full_response.c_str(), full_response.size(), 0);
}

std::string PrometheusExporter::BuildMetricsResponse() {
  // Acquire mutex to ensure consistent metric readings.
  std::lock_guard<std::mutex> lock(export_mutex_);
  // Use stringstream to efficiently build the response string.
  std::ostringstream oss;

  // Iterate through each backend server to gather and format its metrics.
  for (const auto& backend : backends_) {
    const std::string id = backend->Ip();

    // Format metrics for total requests.
    oss << "# HELP backend_requests_total Total requests sent to backend\n";
    oss << "# TYPE backend_requests_total counter\n";
    oss << "backend_requests_total{backend=\"" << id << "\"} "
        << metrics_collector_->GetRequestCount(backend) << "\n";

    // Format metrics for total successful responses.
    oss << "# HELP backend_successes_total Total successful responses from "
           "backend\n";
    oss << "# TYPE backend_successes_total counter\n";
    oss << "backend_successes_total{backend=\"" << id << "\"} "
        << metrics_collector_->GetSuccessCount(backend) << "\n";

    // Format metrics for total failed responses.
    oss << "# HELP backend_failures_total Total failed responses from "
           "backend\n";
    oss << "# TYPE backend_failures_total counter\n";
    oss << "backend_failures_total{backend=\"" << id << "\"} "
        << metrics_collector_->GetFailureCount(backend) << "\n";

    // Format metrics for average latency.
    oss << "# HELP backend_avg_latency_milliseconds Average latency per "
           "backend\n";
    oss << "# TYPE backend_avg_latency_milliseconds gauge\n";
    oss << "backend_avg_latency_milliseconds{backend=\"" << id << "\"} "
        << metrics_collector_->GetAverageLatency(backend) << "\n";
  }

  return oss.str();
}

}  // namespace monitor
}  // namespace load_balancer

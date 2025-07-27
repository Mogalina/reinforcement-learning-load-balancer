#ifndef LOAD_BALANCER_MONITOR_PROMETHEUS_EXPORTER_H_
#define LOAD_BALANCER_MONITOR_PROMETHEUS_EXPORTER_H_

#include "metrics/metrics_collector.h"
#include "core/backend_server.h"

#include <thread>
#include <mutex>
#include <atomic>
#include <vector>
#include <memory>
#include <netinet/in.h>
#include <unordered_map>

namespace load_balancer {
namespace monitor {

// Exposes collected load balancer metrics in Prometheus format.
// This class runs a simple HTTP server that, when queried, provides
// current metrics about backend server performance.
class PrometheusExporter {
 public:
  explicit PrometheusExporter(
      int port,
      std::shared_ptr<MetricsCollector> metrics_collector,
      const std::vector<std::shared_ptr<core::BackendServer>>& backends);

  ~PrometheusExporter();

  // Starts the Prometheus exporter server in a new thread.
  void Start();
  // Stops the Prometheus exporter server gracefully.
  void Stop();

 private:
  // The main server loop for accepting client connections.
  // Runs in a dedicated thread, accepting and handing off incoming HTTP
  // requests.
  void Serve();

  // Handles a single client connection, serving metrics.
  // Reads the incoming request (though largely ignored for Prometheus), builds
  // the metrics response, and sends it back to the client.
  void HandleClient(int client_socket);

  // Builds the Prometheus-formatted metrics response string.
  // Iterates through all backends and retrieves their metrics from the
  // MetricsCollector, formatting them as Prometheus exposition format.
  std::string BuildMetricsResponse();

  // The TCP port number the exporter listens on.
  int port_;
  // The file descriptor for the exporter's listening socket.
  int server_socket_;
  // The thread running the 'Serve' method.
  std::thread server_thread_;
  // Atomic flag to control the server's running state.
  std::atomic<bool> running_;

  // Shared pointer to the MetricsCollector for data access.
  std::shared_ptr<MetricsCollector> metrics_collector_;
  // List of backend servers whose metrics are exported.
  std::vector<std::shared_ptr<core::BackendServer>> backends_;

  // Mutex to protect shared resources during metric export generation.
  std::mutex export_mutex_;
};

}  // namespace monitor
}  // namespace load_balancer

#endif  // LOAD_BALANCER_MONITOR_PROMETHEUS_EXPORTER_H_

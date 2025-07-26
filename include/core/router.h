#ifndef LOAD_BALANCER_ROUTER_H
#define LOAD_BALANCER_ROUTER_H

#include "backend_server.h"
#include "rl/agent.h"

#include <memory>

namespace load_balancer {
namespace core {

// Manages the selection of backend servers for incoming requests.
// This class uses a reinforcement learning agent to intelligently pick the most
// suitable backend server from a pool of available servers.
class Router {
 public:
  explicit Router(std::shared_ptr<rl::Agent> agent);

  void AddBackendServer(std::shared_ptr<BackendServer> backend_server);

  // Selects an available backend server using the configured RL agent.
  std::shared_ptr<BackendServer> PickBackendServer();

 private:
  // Collection of managed backend servers.
  std::vector<std::shared_ptr<BackendServer>> backend_servers_;
  // The reinforcement learning agent for server selection.
  std::shared_ptr<rl::Agent> agent_;
};

}  // namespace core
}  // namespace load_balancer

#endif  // LOAD_BALANCER_ROUTER_H

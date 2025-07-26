#include "core/router.h"

namespace load_balancer {
namespace core {

Router::Router(std::shared_ptr<rl::Agent> agent) : agent_(std::move(agent)) {}

void Router::AddBackendServer(std::shared_ptr<BackendServer> backend_server) {
  backend_servers_.push_back(std::move(backend_server));
}

std::shared_ptr<BackendServer> Router::PickBackendServer() {
  if (backend_servers_.empty()) return nullptr;
  int selected_index = agent_->SelectAction(backend_servers_);
  return backend_servers_.at(selected_index);
}

}  // namespace core
}  // namespace load_balancer

#include "process_manager.hpp"

ProcessManager::ProcessManager() = default;

void ProcessManager::addProcess(Process &process) {
  workers.push_back(std::async(std::launch::async, &Process::asyncTask,
                               std::ref(process), std::ref(shouldRun)));
}

void ProcessManager::start() { shouldRun = true; }

void ProcessManager::stop() { shouldRun = false; }

void ProcessManager::joinWorkers() {
  for (auto it = workers.rbegin(); it != workers.rend(); ++it) {
    if (it->valid())
      it->wait();
  }
  workers.clear();
}

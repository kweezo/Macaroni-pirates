#pragma once

#include <atomic>
#include <future>
#include <vector>

#include "process.hpp"

class ProcessManager {
public:
  ProcessManager();
  void addProcess(Process &process);

  void start();
  void stop();
  void joinWorkers();

private:
  std::atomic_bool shouldRun;
  std::vector<std::future<void>> workers;
};

#pragma once

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
  bool shouldRun = false;
  std::vector<std::future<void>> workers;
};

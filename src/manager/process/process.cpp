#include "process.hpp"

#include "manager/game_state.hpp"

#include <chrono>
#include <thread>

Process::Process() : minSleepNS(0), lastRun(0) {}

Process::Process(uint32_t minSleepNS) : minSleepNS(minSleepNS), lastRun(0) {}

void Process::asyncTask(const bool &shouldRun) {
  try {
    init();
    while (shouldRun) {
      if (lastRun != 0 && minSleepNS > 0) {
        std::this_thread::sleep_for(std::chrono::nanoseconds(minSleepNS));
        if (!shouldRun)
          break;
      }
      run();
      lastRun = NS;
    }
    destruct();
  } catch (const std::exception &e) {
    std::cerr << e.what() << "\n";
    exit(EXIT_FAILURE);
  }
}

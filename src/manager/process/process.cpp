#include "process.hpp"

#include "manager/game_state.hpp"

Process::Process() {

}

Process::Process(uint32_t minSleepNS): minSleepNS(minSleepNS) {

}

void Process::asyncTask(const std::atomic_bool& shouldRun) {
    try {
        init();
        while(shouldRun) {
            if(lastRun + minSleepNS > NS)
                std::this_thread::sleep_for(std::chrono::nanoseconds(minSleepNS - (NS - lastRun))); //TODO what happens on negative time?
            run();
            lastRun = NS;
        } 
        destruct();
    } catch (std::exception e) {
        std::cerr << e.what() << std::endl;
    }
}
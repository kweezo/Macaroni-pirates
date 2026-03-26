#include "process.hpp"

Process::Process() {

}

void Process::asyncTask(const std::atomic_bool& shouldRun) {
    try {
        init();
        while(shouldRun) run();
        destruct();
    } catch (std::exception e) {
        std::cerr << e.what() << std::endl;
    }
}
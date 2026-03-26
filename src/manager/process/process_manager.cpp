#include "process_manager.hpp"

ProcessManager::ProcessManager(): shouldRun(false) {

}

void ProcessManager::addProcess(Process& process) {

    static std::array<std::future<void>, 128> futureBuff = {};
    //SHIT FUCKING RAII OMG
    //FUTURE BLOCKS ON DESTRUCTION FOR ASYNC TO FINISH SO I HAVE TO STORE IT SOMEWHERE JUST TO DO FUCKALL WITH IT

    std::future<void> future = std::async(&Process::asyncTask, std::ref(process), std::ref(shouldRun)); 
    for(std::future<void>& e : futureBuff) { // the future thingamajig of pointlessness
        if(e.valid()) {
            std::future_status stat = e.wait_for(std::chrono::seconds(0));

            if(stat == std::future_status::ready) {
                e.get();
                e = {};
            }
        }

        if(!e.valid()) {  //the future can become invalid in previous statement so a separate validity check here is necessary
            //looks goffy but trust trust
            e = std::move(future);
            return;
        } 
    }

    throw new std::runtime_error("No free future buffers");
}

void ProcessManager::start() {
    shouldRun = true;
}

void ProcessManager::stop() {
    shouldRun = false;
}
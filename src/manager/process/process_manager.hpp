#pragma once

#include <unordered_map>
#include <tuple>
#include <future>
#include <array>

#include "process.hpp"

class ProcessManager {
    public:
        ProcessManager();
        void addProcess(Process& process);

        void start();
        void stop();
    private:
        std::atomic_bool shouldRun;
};
#pragma once

#include <memory>
#include <atomic>
#include <iostream>

class Process {
    friend class ProcessManager;
    protected:
        virtual void init() {};
        virtual void run() {};
        virtual void destruct() {};

        void asyncTask(const std::atomic_bool& shouldRun);
    public:
        std::string getName();

        Process();
        Process(Process&) = delete;
        Process(Process&&) = delete;
};
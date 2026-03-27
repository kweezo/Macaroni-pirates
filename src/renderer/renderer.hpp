#pragma once

#include <SDL3/SDL.h>

#include <array>
#include <functional>
#include <mutex>

#include "manager/process/process.hpp"

#define DRAW_STACK_SIZE 64

class Renderer : public Process {
    void init() override;
    void run() override;
    void destruct() override;

    public:
        Renderer();

        size_t getNextFreeDrawIndex(std::function<void(SDL_Surface*)> fun);
        void removeDrawIndex(size_t i);

        SDL_Surface* getWindowSurface();
    private:
        void createWindow();
        void createRenderer();
        
        void render();

        SDL_Window* window;
        SDL_Renderer* renderer;

        std::array<std::function<void(SDL_Surface*)>, DRAW_STACK_SIZE> drawStack;
        std::mutex drawStackMutex;
};
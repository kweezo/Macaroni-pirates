#pragma once

#include <SDL3/SDL.h>

#include <array>
#include <functional>
#include <future>
#include <mutex>

#include "manager/process/process.hpp"

#define DRAW_STACK_SIZE 64

class Drawable; //fucking disgusting I have to do this but necessary

class Renderer : public Process {
    void init() override;
    void run() override;
    void destruct() override;

    public:
        Renderer();

        size_t getNextFreeDrawIndex(Drawable* drawable);
        void removeDrawIndex(size_t i);

        SDL_Surface* getWindowSurface();
    private:
        void createWindow();
        void createRenderer();
        
        void render();

        SDL_Window* window;
        SDL_Renderer* renderer;

        std::array<Drawable*, DRAW_STACK_SIZE> drawStack;
        std::array<std::future<void>, DRAW_STACK_SIZE> drawFutures;
        std::mutex drawStackMutex;
};
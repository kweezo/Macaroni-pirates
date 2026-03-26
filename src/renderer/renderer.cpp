#include "renderer.hpp"

#include "manager/game_state.hpp"

Renderer::Renderer() {

}

void Renderer::init() {
    SDL_Init(SDL_INIT_VIDEO);

    createWindow();
    createRenderer();
}

void Renderer::run() {
    SDL_Event e;
    while(SDL_PollEvent(&e)) {
        if(e.type == SDL_EVENT_QUIT)
            gameState.processManager.stop();
    }

    SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
    SDL_RenderClear(renderer);

    render();

    SDL_RenderPresent(renderer);
}

void Renderer::destruct() {
    if(renderer)
        SDL_DestroyRenderer(renderer);
    if(window)
        SDL_DestroyWindow(window);
    if(SDL_WasInit(SDL_INIT_VIDEO))
        SDL_Quit();
}

void Renderer::createWindow() {
    window = SDL_CreateWindow("DOWN WITH THE MAC REGIME", 1280, 720, 0);


    if(!window) throw new std::runtime_error("Failed to create window");
}

void Renderer::createRenderer() {
    renderer = SDL_CreateRenderer(window, nullptr);
    
    if(!renderer) throw new std::runtime_error("Failed to create renderer");
}

void Renderer::render() {
    std::array<std::function<void(SDL_Renderer*)>, DRAW_STACK_SIZE> drawStackCopy;

    drawStackMutex.lock();
    std::copy(drawStack.begin(), drawStack.end(), drawStackCopy.begin()); 
    drawStackMutex.unlock();

    for(auto drawFunc : drawStackCopy) {
        if(drawFunc)
            drawFunc(renderer);
    }
}

size_t Renderer::getNextFreeDrawIndex(std::function<void(SDL_Renderer*)> fun) {
    std::lock_guard lock(drawStackMutex);
    for(int i = 0; i < DRAW_STACK_SIZE; i++) {
        if(drawStack[i]) continue;
        
        drawStack[i] = fun;
        return i;
    }

    throw new std::runtime_error("Out of free rendeer slots");
}

void Renderer::removeDrawIndex(size_t i) {
    std::lock_guard lock(drawStackMutex);
    drawStack[i] = {};
}
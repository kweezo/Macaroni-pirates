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

    memset(getWindowSurface()->pixels, 127, getWindowSurface()->pitch * getWindowSurface()->h);

    render();

    SDL_UpdateWindowSurface(window);
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
    window = SDL_CreateWindow("this shit is so async", WINDOW_WIDTH, WINDOW_HEIGHT, 0);


    if(!window) throw new std::runtime_error("Failed to create window");
}

void Renderer::createRenderer() {

    renderer = SDL_CreateSoftwareRenderer(getWindowSurface());
    if(!renderer) throw new std::runtime_error("Failed to create renderer");
}

void Renderer::render() {
    std::array<Drawable*, DRAW_STACK_SIZE> drawStackCopy;

    drawStackMutex.lock();
    std::copy(drawStack.begin(), drawStack.end(), drawStackCopy.begin()); 
    drawStackMutex.unlock();

    for(int i = 0; i < DRAW_STACK_SIZE; i++) {
        if(drawStack[i])
            drawFutures[i] = std::async(
                std::bind(&Drawable::renderWrapper, drawStackCopy[i], getWindowSurface()));
    }

    for(int i = 0; i < DRAW_STACK_SIZE; i++) {
        if(drawFutures[i].valid())
            drawFutures[i].get();
    }
    for(int i = 0; i < DRAW_STACK_SIZE; i++) {
        if(drawStack[i])
            drawStack[i]->resetMutex();
    }
  
}

size_t Renderer::getNextFreeDrawIndex(Drawable* drawable) {
    std::lock_guard lock(drawStackMutex);
    for(int i = 0; i < DRAW_STACK_SIZE; i++) {
        if(drawStack[i]) continue;
        
        drawStack[i] = drawable;
        return i;
    }

    throw new std::runtime_error("Out of free rendeer slots");
}

void Renderer::removeDrawIndex(size_t i) {
    std::lock_guard lock(drawStackMutex);
    drawStack[i] = {};
}

SDL_Surface* Renderer::getWindowSurface() {
    return SDL_GetWindowSurface(window);
}
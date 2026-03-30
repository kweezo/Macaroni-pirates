#include "drawable.hpp"

#include "manager/game_state.hpp"

Drawable::Drawable() {
    mutex.lock();
    drawIndex = 
     gameState.renderer.getNextFreeDrawIndex(this);
}

Drawable::~Drawable() {
    gameState.renderer.removeDrawIndex(drawIndex);

    for(Drawable* drawable : dependees) {
        if(drawable)
            drawable->removeDependency(this);
    }
}

void Drawable::addDependency(Drawable* drawable) {
    for(Drawable*& dep :dependencies) {
        if(dep) continue;

        dep = drawable;
        drawable->addDependee(this);
        return;
    }

    throw new std::runtime_error("No free dependency slots left");
}

void Drawable::removeDependency(Drawable* drawable) {
    for (Drawable*& dep : dependencies) {
        if(dep != drawable) continue;

        dep = nullptr;
        drawable->removeDependee(this);
        return;
    }

    throw new std::runtime_error("Tried to remove non-existent dependency");
}

void Drawable::addDependee(Drawable* drawable) {
    for(Drawable*& dep : dependees) {
        if(dep) continue;

        dep = drawable;
        return;
    }

    throw new std::runtime_error("No free dependee slots left");
}

void Drawable::removeDependee(Drawable* drawable) {
    for (Drawable*& dep : dependees) {
        if(dep != drawable) continue;

        dep = nullptr;
        return;
    }

    throw new std::runtime_error("Tried to remove non-existent dependee");
}

void Drawable::renderWrapper(SDL_Surface* surface) {
    for(Drawable* drawable : dependencies) {
        if(!drawable) continue;
        drawable->getMutex()->lock();
        drawable->getMutex()->unlock();
    }
    render(surface);
    mutex.unlock();
}

void Drawable::resetMutex() {
    mutex.lock();
}

std::mutex* Drawable::getMutex() {
    return &mutex;
}
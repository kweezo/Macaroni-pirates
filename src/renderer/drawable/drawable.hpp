#pragma once

#include <SDL3/SDL.h>

#include <mutex>
#include <array>

#define MAX_DRAWABLE_DEPENDENCY_COUNT 8

class Drawable {
    public:
        Drawable();
        ~Drawable();

        void renderWrapper(SDL_Surface* surface);
        void resetMutex();
        std::mutex* getMutex();

        void addDependency(Drawable* drawable);
        void removeDependency(Drawable* drawable);

    protected:
        virtual void render(SDL_Surface* surface) {}
    private:
        void addDependee(Drawable* drawable);
        void removeDependee(Drawable* drawable);

        std::mutex mutex;
        uint32_t drawIndex;

        std::array<Drawable*, MAX_DRAWABLE_DEPENDENCY_COUNT> dependencies;
        std::array<Drawable*, MAX_DRAWABLE_DEPENDENCY_COUNT> dependees;
};
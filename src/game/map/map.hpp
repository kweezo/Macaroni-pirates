#pragma once
#include <SDL3/SDL.h>

#include "manager/process/process.hpp"
#include "texture/texture.hpp"

class Map : public Process {
    public:
        Map();
        Map(float beachRatio);

    private:
        void init() override;
        void run() override;
        void destruct() override;

        void render(SDL_Surface* surface);
        
        Texture sandTex;
        size_t drawIndex;

        float beachRatio;
};
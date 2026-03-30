#pragma once
#include <SDL3/SDL.h>

#include "manager/process/process.hpp"
#include "texture/texture.hpp"
#include "renderer/drawable/drawable.hpp"

class Map : public Process, Drawable {
    public:
        Map();
        Map(float beachRatio);

    private:
        void init() override;
        void run() override;
        void destruct() override;

        void render(SDL_Surface* surface) override;
        void render_row(SDL_Surface* surface, uint16_t y);
        
        Texture sandTex;

        float beachRatio;
};
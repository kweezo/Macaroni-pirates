#pragma once

#include <SDL3/SDL.h>

#include <cmath>

#include "manager/process/process.hpp"
#include "texture/texture.hpp"

class Player : public Process {
    public:
        Player();
    private:
        void init() override;
        void run() override;
        void destruct() override;

        void render(SDL_Renderer* renderer);

        void movement();

        Texture tex;
        size_t drawIndex;

        float x, y;

        const float speed = 0.4;

        float dt;
        size_t lastTime;
};
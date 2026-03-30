#pragma once

#include <SDL3/SDL.h>

#include <cmath>
#include <array>

#include "manager/process/process.hpp"
#include "texture/texture.hpp"
#include "renderer/drawable/drawable.hpp"
#include "cannonball/cannonball.hpp"

#define MAX_CANNONBALL_COUNT 12
#define CANNONBALL_OFFSET_X 100
#define CANNONBALL_OFFSET_Y 70
#define BASE_CANNONBALL_SPEED 1

class Player : public Process, Drawable {
    public:
        Player();
    private:
        void init() override;
        void run() override;
        void destruct() override;

        void render(SDL_Surface* surface) override;

        void movement();
        void shooting();

        Texture tex;

        float x, y;

        const float speed = 0.4;

        bool lastShotPress;

        float dt;
        size_t lastTime;

        std::array<Cannonball, MAX_CANNONBALL_COUNT> cannonballs;
};
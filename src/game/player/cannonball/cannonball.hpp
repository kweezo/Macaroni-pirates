#pragma once

#include "texture/texture.hpp"

#include "macros.hpp"

class Cannonball {
    public:
        Cannonball();
        Cannonball(float x, float y, float speed);

        void update(float dt);
        void draw(SDL_Surface* surface);

        bool isActive();
    private:
        void movement(float dt);
        void boundsCheck();

        float x, y;
        float speed;
        bool active;

        Texture tex;
};
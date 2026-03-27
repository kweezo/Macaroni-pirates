#pragma once

#include <cstdint>
#include <cmath>

#include <SDL3/SDL.h>

class Texture {
    public:
        struct Transform {
            int x, y;
            uint16_t w, h;
        };

        struct Color {
            uint8_t r, g, b, a;
        };

        Texture();
        Texture(const uint8_t* data, uint8_t w, uint8_t h);

        void draw(SDL_Surface* surface, Transform transform, Color color);

    private:
        uint8_t const* data;
        uint8_t w;
        uint8_t h;
};
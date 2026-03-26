#pragma once

#include <cstdint>

#include <SDL3/SDL.h>

class Texture {
    public:
        struct Transform {
            uint16_t x, y, w, h;
        };

        struct Color {
            uint8_t r, g, b, a;
        };

        Texture();
        Texture(const uint8_t* data, uint8_t w, uint8_t h);

        void draw(SDL_Renderer* renderer, Transform transform, Color color);

    private:
        uint8_t const* data;
        uint8_t w;
        uint8_t h;
};
#include "texture.hpp"

Texture::Texture(): data(nullptr), w(), h() {

}

Texture::Texture(const uint8_t* data, uint8_t w, uint8_t h): data(data), w(w), h(h) {

}

#include <iostream>

void Texture::draw(SDL_Surface* surface, Transform transform, Color color) {
    float pWidth = ceil((float)transform.w / w);
    float pHeight = ceil((float)transform.h / h);

    for(uint8_t x = 0; x < w; x++) {
        for(uint8_t y = 0; y < h; y++) {
            SDL_Rect rect{};
            rect.x = floor(transform.x + x * pWidth);
            rect.y = floor(transform.y + y * pHeight);
            rect.w = pWidth;
            rect.h = pHeight;
            if(rect.x < 0 || rect.y < 0) continue;
            for(uint8_t py = 0; py < rect.h; py++) {
                size_t dstOffset = (size_t)(rect.x * 4 + floor(py + rect.y) * surface->pitch);
                size_t srcOffset = ((size_t)floor(x + y * w) * 4);
                if(dstOffset >= surface->h * surface->pitch - 4 || srcOffset >= w * h * 4 - 4) continue;
                if(!data[(size_t)floor((x + y * w) * 4 + 3)]) continue; //transparent
                memcpy((void*)((size_t)surface->pixels + dstOffset),
                     data + srcOffset, 4 * rect.w);
                
            }
        }
    }

}
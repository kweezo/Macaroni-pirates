#include "texture.hpp"

Texture::Texture(): data(nullptr), w(), h() {

}

Texture::Texture(const uint8_t* data, uint8_t w, uint8_t h): data(data), w(w), h(h) {

}

void Texture::draw(SDL_Renderer* renderer, Transform transform, Color color) {
    float pWidth = (float)transform.w / w;
    float pHeight = (float)transform.h / h;

    for(uint8_t x = 0; x < w; x++) {
        for(uint8_t y = 0; y < h; y++) {
            SDL_FRect rect{};

            rect.x = transform.x + x * pWidth;
            rect.y = transform.y + y * pHeight;
            rect.w = pWidth;
            rect.h = pHeight;

            SDL_SetRenderDrawColorFloat(renderer,
                 data[y * w + x] / 255.0 * color.r / 255.0,
                 data[y * w + x + 1] / 255.0 * color.g / 255.0,
                 data[y * w + x + 2] / 255.0 * color.b / 255.0,
                 color.a / 255.0);


            SDL_RenderFillRect(renderer, &rect);
        }
    }
}
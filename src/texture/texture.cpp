#include "texture.hpp"

std::array<std::mutex, RENDER_CHUNK_COUNT_X * RENDER_CHUNK_COUNT_Y> Texture::mutexes = {};

Texture::Texture(): data(nullptr), w(), h() {

}

Texture::Texture(const uint8_t* data, uint8_t w, uint8_t h): data(data), w(w), h(h) {

}

void Texture::draw(SDL_Surface* surface, Transform transform, Color color) {
    float pWidth = ceil((float)transform.w / w);
    float pHeight = ceil((float)transform.h / h);

    toggleMutexArray(transform, true);

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

    toggleMutexArray(transform, false);
}

void Texture::toggleMutexArray(Transform transform, bool lock) {
    size_t mutexLockCountX = ceil((float)transform.w / RENDER_CHUNK_SIZE);
    size_t mutexLockCountY = ceil((float)transform.h / RENDER_CHUNK_SIZE);
    int startX = floor(transform.x / RENDER_CHUNK_SIZE);
    int startY = floor(transform.y / RENDER_CHUNK_SIZE);

    for(int x = startX; x < startX + mutexLockCountX; x++) {
        for(int y = startY; y < startY + mutexLockCountY; y++) {
            lock ?
            mutexes[y * RENDER_CHUNK_COUNT_X + x].lock() :
            mutexes[y * RENDER_CHUNK_COUNT_X + x].unlock();
        }
    }
}
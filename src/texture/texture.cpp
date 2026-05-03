#include "texture.hpp"

#include <cstring>

std::array<std::mutex, RENDER_CHUNK_COUNT_X * RENDER_CHUNK_COUNT_Y> Texture::mutexes =
    {};

Texture::Texture() = default;

Texture::Texture(const uint8_t *data, int width, int height)
    : data(data), w(width), h(height) {}

void Texture::draw(SDL_Surface *surface, Transform transform, Color) {
  if (!data || w <= 0 || h <= 0 || !surface || !surface->pixels)
    return;

  const float pWidth = std::ceil((float)transform.w / (float)w);
  const float pHeight = std::ceil((float)transform.h / (float)h);

  setMutexArray(transform, true);

  for (int x = 0; x < w; x++) {
    for (int y = 0; y < h; y++) {
      SDL_Rect rect{};
      rect.x = (int)std::floor(transform.x + (float)x * pWidth);
      rect.y = (int)std::floor(transform.y + (float)y * pHeight);
      rect.w = (int)pWidth;
      rect.h = (int)pHeight;
      if (rect.x < 0 || rect.y < 0)
        continue;
      for (int py = 0; py < rect.h; py++) {
        const size_t dstOffset =
            (size_t)(rect.x * 4 + (size_t)std::floor((float)py + rect.y) *
                                       (size_t)surface->pitch);
        const size_t srcOffset = (size_t)(x + y * w) * 4u;
        if (dstOffset >= (size_t)surface->h * (size_t)surface->pitch - 4 ||
            srcOffset >= (size_t)w * (size_t)h * 4u - 4)
          continue;
        if (!data[srcOffset + 3])
          continue;
        std::memcpy((void *)((size_t)surface->pixels + dstOffset),
                    data + srcOffset, 4u * (size_t)rect.w);
      }
    }
  }

  setMutexArray(transform, false);
}

void Texture::setMutexArray(Transform transform, bool lock) {
  const size_t mutexLockCountX =
      (size_t)std::ceil((float)transform.w / RENDER_CHUNK_SIZE);
  const size_t mutexLockCountY =
      (size_t)std::ceil((float)transform.h / RENDER_CHUNK_SIZE);
  const int startX = (int)std::floor((float)transform.x / RENDER_CHUNK_SIZE);
  const int startY = (int)std::floor((float)transform.y / RENDER_CHUNK_SIZE);

  for (int x = startX; x < startX + (int)mutexLockCountX; x++) {
    for (int y = startY; y < startY + (int)mutexLockCountY; y++) {
      if (x < 0 || y < 0 ||
          (size_t)y >= RENDER_CHUNK_COUNT_Y || (size_t)x >= RENDER_CHUNK_COUNT_X)
        continue;
      lock ? mutexes[(size_t)y * RENDER_CHUNK_COUNT_X + (size_t)x].lock()
           : mutexes[(size_t)y * RENDER_CHUNK_COUNT_X + (size_t)x].unlock();
    }
  }
}

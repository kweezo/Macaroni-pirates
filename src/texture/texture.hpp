#pragma once

#include <cstdint>
#include <cmath>
#include <mutex>
#include <algorithm>
#include <array>

#include <SDL3/SDL.h>

#include "macros.hpp"

#define RENDER_CHUNK_SIZE 90
#define RENDER_CHUNK_COUNT_X ((size_t)std::ceil(WINDOW_WIDTH / RENDER_CHUNK_SIZE) + 1)
#define RENDER_CHUNK_COUNT_Y ((size_t)std::ceil(WINDOW_HEIGHT / RENDER_CHUNK_SIZE) + 1)

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
  Texture(const uint8_t *data, int width, int height);

  void draw(SDL_Surface *surface, Transform transform, Color color);

  void drawSprite(SDL_Surface *surface, Transform transform, Color color) {
    draw(surface, transform, color);
  }

  const uint8_t *bytes() const { return data; }
  int widthPx() const { return w; }
  int heightPx() const { return h; }
  int rowPitch() const { return w * 4; }

private:
  uint8_t const *data = nullptr;
  int w = 0;
  int h = 0;

  static std::array<std::mutex, RENDER_CHUNK_COUNT_X * RENDER_CHUNK_COUNT_Y>
      mutexes;

  void setMutexArray(Transform transform, bool lock);
};

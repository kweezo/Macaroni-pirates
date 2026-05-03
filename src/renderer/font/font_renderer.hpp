#pragma once

#include <cmath>
#include <cstdint>
#include <cstring>

struct SDL_Surface;

class FontRenderer {
public:
  static void drawText(SDL_Surface *surface, float x, float y, float scale,
                       const char *text, uint8_t r = 255, uint8_t g = 255,
                       uint8_t b = 255);

  static float estimateTextWidth(const char *text, float scale);
};

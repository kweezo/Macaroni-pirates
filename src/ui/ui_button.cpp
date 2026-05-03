#include "ui/ui_button.hpp"

#include "renderer/font/font_renderer.hpp"

#include <SDL3/SDL.h>

#include <algorithm>
#include <cmath>

UIButton::UIButton(float ax, float ay, float aw, float ah)
    : x(ax), y(ay), w(aw), h(ah) {}

bool UIButton::hitTest(float px, float py) const {
  return px >= x && px < x + w && py >= y && py < y + h;
}

void UIButton::draw(SDL_Surface *surface, const char *label,
                    float textScale) const {
  if (!surface || !label)
    return;

  const SDL_PixelFormatDetails *fmt =
      SDL_GetPixelFormatDetails(surface->format);
  if (!fmt)
    return;

  const uint32_t fill =
      SDL_MapRGBA(fmt, nullptr, 48, 56, 72, (uint8_t)230);
  const uint32_t border =
      SDL_MapRGBA(fmt, nullptr, 120, 140, 180, (uint8_t)255);

  SDL_Rect outer{(int)std::floor(x), (int)std::floor(y), (int)std::floor(w),
                 (int)std::floor(h)};
  SDL_FillSurfaceRect(surface, &outer, border);
  SDL_Rect inner{(int)std::floor(x + 2), (int)std::floor(y + 2),
                 (int)std::floor(w - 4), (int)std::floor(h - 4)};
  SDL_FillSurfaceRect(surface, &inner, fill);

  const float tw = FontRenderer::estimateTextWidth(label, textScale);
  const float tx = x + (w - tw) * 0.5f;
  const float ty = y + (h - textScale * 32.0f) * 0.5f;
  FontRenderer::drawText(surface, tx, ty, textScale, label, 230, 235, 245);
}

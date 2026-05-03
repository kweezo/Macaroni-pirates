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

  const float hPad = 8.0f;
  const float maxLabelW = std::max(8.0f, w - hPad);
  float s = textScale;
  float tw = FontRenderer::estimateTextWidth(label, s);
  for (int iter = 0; iter < 10 && tw > maxLabelW && s > 0.14f; ++iter) {
    s *= 0.9f;
    tw = FontRenderer::estimateTextWidth(label, s);
  }

  float tx = x + (w - tw) * 0.5f;
  const float minTx = x + 4.f;
  if (tx < minTx)
    tx = minTx;
  float clipW = x + w - 4.f - tx;
  if (clipW < 8.f)
    clipW = maxLabelW;

  const float ty = y + (h - s * 32.0f) * 0.5f;
  FontRenderer::drawText(surface, tx, ty, s, label, 230, 235, 245, clipW);
}

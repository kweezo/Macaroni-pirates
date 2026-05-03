#include "ui/ui_text_field.hpp"

#include "renderer/font/font_renderer.hpp"

#include <SDL3/SDL.h>

#include <algorithm>
#include <cmath>
#include <cstring>

namespace {

void copyAsciiPrintable(char *dest, size_t destCap, const char *utf8) {
  dest[0] = '\0';
  if (!utf8 || !utf8[0] || destCap == 0)
    return;
  size_t w = 0;
  for (size_t i = 0; utf8[i] && w + 1 < destCap; ++i) {
    const unsigned char c = (unsigned char)utf8[i];
    if (c < 32 || c > 126)
      continue;
    dest[w++] = (char)c;
  }
  dest[w] = '\0';
}

}

void UITextField::setRect(float ax, float ay, float aw, float ah) {
  x = ax;
  y = ay;
  w = aw;
  h = ah;
}

void UITextField::setWindow(SDL_Window *window) { sdlWindow = window; }

void UITextField::setText(const char *utf8) {
  copyAsciiPrintable(textChars, MaxChars + 1, utf8);
}

void UITextField::setPlaceholder(const char *utf8) {
  copyAsciiPrintable(placeholderChars, MaxChars + 1, utf8);
}

bool UITextField::hitTest(float px, float py) const {
  return px >= x && px < x + w && py >= y && py < y + h;
}

void UITextField::draw(SDL_Surface *surface, float textScale) const {
  if (!surface)
    return;
  const SDL_PixelFormatDetails *fmt =
      SDL_GetPixelFormatDetails(surface->format);
  if (!fmt)
    return;

  const uint32_t bg =
      SDL_MapRGBA(fmt, nullptr, isFocused ? 40 : 28, isFocused ? 44 : 32,
                  isFocused ? 58 : 44, (uint8_t)255);

  const int xi = (int)std::floor(x);
  const int yi = (int)std::floor(y);
  const int wi = (int)std::floor(w);
  const int hi = (int)std::floor(h);
  SDL_Rect r{xi, yi, wi, hi};
  SDL_FillSurfaceRect(surface, &r, bg);

  const uint8_t br = isFocused ? (uint8_t)150 : (uint8_t)92;
  const uint8_t bgc = isFocused ? (uint8_t)168 : (uint8_t)104;
  const uint8_t bb = isFocused ? (uint8_t)190 : (uint8_t)120;
  const uint32_t edge = SDL_MapRGBA(fmt, nullptr, br, bgc, bb, (uint8_t)255);
  const int tk = 2;
  SDL_Rect top{xi, yi, wi, tk};
  SDL_Rect bot{xi, yi + hi - tk, wi, tk};
  SDL_Rect lef{xi, yi, tk, hi};
  SDL_Rect rig{xi + wi - tk, yi, tk, hi};
  SDL_FillSurfaceRect(surface, &top, edge);
  SDL_FillSurfaceRect(surface, &bot, edge);
  SDL_FillSurfaceRect(surface, &lef, edge);
  SDL_FillSurfaceRect(surface, &rig, edge);

  const bool showPlaceholder =
      textChars[0] == '\0' && placeholderChars[0] != '\0';
  const float innerW = std::max(4.f, w - 16.f);
  if (showPlaceholder) {
    FontRenderer::drawText(surface, x + 8.0f, y + 8.0f, textScale,
                           placeholderChars, 110, 118, 132, innerW);
  } else {
    FontRenderer::drawText(surface, x + 8.0f, y + 8.0f, textScale, textChars,
                           220, 225, 235, innerW);
  }
}

bool UITextField::handleMouseDown(float mx, float my) {
  const bool hit = hitTest(mx, my);
  setFocused(hit);
  return hit;
}

void UITextField::setFocused(bool on) {
  if (isFocused == on)
    return;
  isFocused = on;
  if (sdlWindow) {
    if (on)
      SDL_StartTextInput(sdlWindow);
    else
      SDL_StopTextInput(sdlWindow);
  }
}

bool UITextField::handleSDLEvent(const SDL_Event &e) {
  if (!isFocused)
    return false;

  if (e.type == SDL_EVENT_TEXT_INPUT) {
    const char *t = e.text.text;
    if (!t)
      return true;
    for (; *t; ++t) {
      const unsigned char c = (unsigned char)*t;
      if (c < 32 || c > 126)
        continue;
      const size_t len = std::strlen(textChars);
      if (len >= MaxChars)
        break;
      textChars[len] = (char)c;
      textChars[len + 1] = '\0';
    }
    return true;
  }

  if (e.type == SDL_EVENT_KEY_DOWN && e.key.down && !e.key.repeat) {
    if (e.key.key == SDLK_BACKSPACE) {
      const size_t len = std::strlen(textChars);
      if (len > 0) {
        textChars[len - 1] = '\0';
      }
      return true;
    }
  }
  return false;
}

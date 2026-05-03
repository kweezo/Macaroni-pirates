#include "ui/ui_text_field.hpp"

#include "renderer/font/font_renderer.hpp"

#include <SDL3/SDL.h>

#include <cmath>
#include <cstring>

void UITextField::setRect(float ax, float ay, float aw, float ah) {
  x = ax;
  y = ay;
  w = aw;
  h = ah;
}

void UITextField::setWindow(SDL_Window *window) { sdlWindow = window; }

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
  SDL_Rect r{(int)std::floor(x), (int)std::floor(y), (int)std::floor(w),
             (int)std::floor(h)};
  SDL_FillSurfaceRect(surface, &r, bg);

  FontRenderer::drawText(surface, x + 8.0f, y + 8.0f, textScale, textChars,
                         220, 225, 235);
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

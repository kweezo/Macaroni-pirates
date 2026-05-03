#pragma once

#include <SDL3/SDL.h>

#include <cstddef>
#include <cstring>

struct SDL_Surface;

class UITextField {
public:
  static constexpr size_t MaxChars = 63;

  UITextField() = default;

  void setRect(float ax, float ay, float aw, float ah);
  void setWindow(SDL_Window *window);

  bool hitTest(float px, float py) const;
  void draw(SDL_Surface *surface, float textScale) const;

  bool handleMouseDown(float mx, float my);
  bool handleSDLEvent(const SDL_Event &e);

  void setFocused(bool on);
  bool focused() const { return isFocused; }

  void setText(const char *utf8);
  void setPlaceholder(const char *utf8);

  const char *text() const { return textChars; }

private:
  float x = 0;
  float y = 0;
  float w = 0;
  float h = 0;
  SDL_Window *sdlWindow = nullptr;
  char textChars[MaxChars + 1]{};
  char placeholderChars[MaxChars + 1]{};
  bool isFocused = false;
};

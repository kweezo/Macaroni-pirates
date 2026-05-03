#pragma once

struct SDL_Surface;

class UIButton {
public:
  UIButton() = default;
  UIButton(float ax, float ay, float aw, float ah);

  bool hitTest(float px, float py) const;
  void draw(SDL_Surface *surface, const char *label, float textScale) const;

private:
  float x = 0;
  float y = 0;
  float w = 0;
  float h = 0;
};

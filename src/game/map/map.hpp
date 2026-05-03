#pragma once

#include <SDL3/SDL.h>

#include "manager/process/process.hpp"
#include "renderer/drawable/drawable.hpp"
#include "texture/texture.hpp"

class Map : public Process, public Drawable {
public:
  Map();
  explicit Map(float beachRatio);

  void setBeachRatio(float ratio);

  bool onBeach(float x, float y);
  float oceanMaxY() const;

private:
  void init() override;
  void run() override;
  void destruct() override;

  void render(SDL_Surface *surface) override;
  void render_row(SDL_Surface *surface, uint16_t y, int clipMinX, int clipMaxX,
                  int clipMinY, int clipMaxY);

  Texture sandTex;
  float beachRatio = 0.f;
};

#pragma once

#include "manager/process/process.hpp"
#include "renderer/drawable/drawable.hpp"

class AllyManager : public Process, Drawable {
public:
  AllyManager();

  bool onCannonballHit(float x, float y, float w, float h);

private:
  void init() override;
  void run() override;
  void destruct() override;
  void render(SDL_Surface *surface) override;
};

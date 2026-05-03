
#pragma once

#include <SDL3/SDL.h>

#include <array>
#include <cmath>

#include "cannonball/cannonball.hpp"
#include "manager/process/process.hpp"
#include "renderer/drawable/drawable.hpp"
#include "texture/texture.hpp"

#define MAX_CANNONBALL_COUNT 12
#define CANNONBALL_OFFSET_X 100
#define CANNONBALL_OFFSET_Y 70
#define BASE_CANNONBALL_SPEED 1000
#define PLAYER_WIDTH 150
#define PLAYER_HEIGHT 100

class Player : public Process, Drawable {
public:
  Player();
  float centerX() const;
  float centerY() const;

private:
  void init() override;
  void run() override;
  void destruct() override;

  void render(SDL_Surface *surface) override;

  void movement();
  void shooting();
  bool feetOnBeach() const;

  Texture tex;
  Texture texAlt;

  float x, y;

  const float speed = 400;

  bool lastShotPress;

  float dt;
  size_t lastTime;

  std::array<Cannonball, MAX_CANNONBALL_COUNT> cannonballs;
};

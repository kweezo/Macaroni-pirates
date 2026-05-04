#pragma once

#include <array>

#include "manager/process/process.hpp"
#include "renderer/drawable/drawable.hpp"
#include "texture/texture.hpp"

#define MAX_ALLY_COUNT 5
#define ALLY_SPRITE_DIM 52
#define BASE_ALLY_PATROL_SPEED 88

class AllyManager : public Process, public Drawable {
public:
  AllyManager();

  bool cannonballOverlapsAlly(float x, float y, float w, float h);

  void patrolTick(float sharedDt);

private:
  struct Instance {
    float x = 0;
    float y = 0;
    float speedX = 0;
    bool active = false;
  };

  void init() override;
  void run() override;
  void destruct() override;

  void render(SDL_Surface *surface) override;

  void spawnAllies();
  void updatePatrol(Instance &inst);

  Texture tex{};

  std::array<Instance, MAX_ALLY_COUNT> instances{};
  float dt = 0;
  size_t lastTime = 0;
  bool drawLinkedToMap = false;
};

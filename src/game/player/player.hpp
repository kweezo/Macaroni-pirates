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
#define PLAYER_MAX_HEALTH 100
#define PLAYER_DAMAGE_SHOOT_DEPOSITING_TRASH 18

class Player : public Process, public Drawable {
public:
  Player();
  float centerX() const;
  float centerY() const;

  void prepareForNewRound();
  void placeRandomTopSpawn();
  void takeDamage(int amount);
  int health() const;
  void collisionRect(float &outX, float &outY, float &outW, float &outH) const;

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

  bool drawLinkedToMap = false;

  int healthPoints = PLAYER_MAX_HEALTH;

  // ===== Enemy collision score cooldown =====
  float enemyCollisionPenaltyCooldown = 0.0f;
  static constexpr float ENEMY_COLLISION_PENALTY_INTERVAL = 0.3f;

  std::array<Cannonball, MAX_CANNONBALL_COUNT> cannonballs;
};
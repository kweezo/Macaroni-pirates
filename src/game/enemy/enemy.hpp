#pragma once

#include "manager/process/process.hpp"
#include "renderer/drawable/drawable.hpp"
#include "texture/texture.hpp"

#include <array>
#include <cassert>

#define BASE_ENEMY_SPEED 100
#define ENEMY_SPEED_DEVIATION 50
#define MAX_ENEMY_COUNT 20
#define ENEMY_SIZE 64
#define ENEMY_TRASH_DEPOSIT_TIME (2 * NS)
#define MAX_FLOATING_TRASH 192
#define TRASH_FLOAT_DIM 22
#define TRASH_DRIFT_SPEED 38.f
#define SCORE_POINTS_TRASH_COLLECT 2
#define SCORE_POINTS_ENEMY_KILL 50
#define SCORE_PENALTY_ALLY_HIT 30
#define SCORE_PENALTY_PLAYER_ENEMY_TOUCH 30
#define PLAYER_ENEMY_SCORE_PENALTY_COOLDOWN_NS (400000000ULL)

class EnemyManager : public Process, public Drawable {
public:
  EnemyManager();

  int hitEnemyByRect(float x, float y, float w, float h, int maxHits);
  void hitTrashByRect(float x, float y, float w, float h);
  void applyPlayerEnemyScorePenalty(float px, float py, float pw, float ph);
  bool cannonballDissolvesTrash(float x, float y, float w, float h);
  void applyCollisionWithAlly(float ax, float ay, float aw, float ah,
                              float &allySpeedX);
  void spawnStageWave();
  bool cannonballOverlapsDepositingEnemy(float x, float y, float w, float h);

private:
  struct TrashPiece {
    float x = 0;
    float y = 0;
    float vx = 0;
    float vy = 0;
    bool active = false;
  };

  struct Instance {
    size_t oceanTouchTime;
    uint32_t leftNeighbor = 0;
    uint32_t rightNeighbor = 0;
    uint64_t lastTrashSpawnNano = 0;
    float x = 0;
    float y = 0;
    float speed = 0;
    bool dir = false;
    bool active = false;
    bool invulnerable = false;
  };

  void init() override;
  void run() override;
  void destruct() override;
  void render(SDL_Surface *surface) override;

  void spawnInstances(size_t count, float speedMultiplier);
  void tryAdvanceEnemyWaveLocked();
  void updateInstance(Instance &instance);
  bool depositing(Instance const &instance) const;

  void clearFloatingTrash();
  void simulateFloatingTrash();
  void spawnFloatingTrash(Instance const &instance);

  Texture tex;
  Texture invTex;
  Texture trashTex;
  std::array<Instance, MAX_ENEMY_COUNT> instances{};
  std::array<TrashPiece, MAX_FLOATING_TRASH> floatingTrash{};

  float dt = 0;
  size_t lastTime = 0;
  bool drawLinkedToMap = false;
  uint64_t lastPlayerEnemyScorePenaltyNs = 0;
};

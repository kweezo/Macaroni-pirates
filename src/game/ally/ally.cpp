#include "game/ally/ally.hpp"

#include "game/ally/ally_tex"

#include "manager/game_state.hpp"
#include "macros.hpp"
#include "math/aabb.hpp"

#include <cmath>
#include <cstdlib>

AllyManager::AllyManager() : Process(0) {}

void AllyManager::init() {
  tex = Texture(allyTexDat, (int)allyTexWidth, (int)allyTexHeight);
  if (!drawLinkedToMap) {
    addDependency((Drawable *)&gameState.map);
    addDependency((Drawable *)&gameState.enemyManager);
    drawLinkedToMap = true;
  }
  lastTime = NANOS;
  spawnAllies();
}

void AllyManager::spawnAllies() {
  instances = {};
  const float waterLine = gameState.map.oceanMaxY();
  const float ySandTop = waterLine + 18.f;
  const float ySandBottom =
      (float)WINDOW_HEIGHT - (float)ALLY_SPRITE_DIM - 20.f;

  for (size_t i = 0; i < MAX_ALLY_COUNT; ++i) {
    Instance &inst = instances[i];
    inst.active = true;
    const int laneDiv =
        (MAX_ALLY_COUNT <= 1) ? 1 : (int)(MAX_ALLY_COUNT - (size_t)1);
    const int colStride =
        (MAX_ALLY_COUNT <= 1) ? 0 : (WINDOW_WIDTH - 280) / laneDiv;
    inst.x = (float)(140 + (int)i * colStride);
    inst.y = ySandTop +
             (ySandBottom - ySandTop) *
                 ((float)i / (float)((MAX_ALLY_COUNT > 1) ? (MAX_ALLY_COUNT - 1)
                                                            : 1));
    inst.speedX = ((i % 2) ? 1.f : -1.f) *
                  (BASE_ALLY_PATROL_SPEED + (float)(rand() % 28 - 14));
  }
}

void AllyManager::updatePatrol(Instance &inst) {
  inst.x += inst.speedX * dt;
  if (inst.x < 0.f) {
    inst.x = 0.f;
    inst.speedX = std::fabs(inst.speedX);
  } else if (inst.x > (float)(WINDOW_WIDTH - ALLY_SPRITE_DIM)) {
    inst.x = (float)(WINDOW_WIDTH - ALLY_SPRITE_DIM);
    inst.speedX = -std::fabs(inst.speedX);
  }
}

void AllyManager::patrolTick(float sharedDt) {
  if (gameState.replay.isReplayActive())
    return;
  if (gameState.paused)
    return;
  if (gameState.isGameOver())
    return;

  dt = sharedDt;
  lastTime = NANOS;

  float px = 0.f;
  float py = 0.f;
  float pw = 0.f;
  float ph = 0.f;
  gameState.player.collisionRect(px, py, pw, ph);

  for (Instance &inst : instances) {
    if (!inst.active)
      continue;
    updatePatrol(inst);
    gameState.enemyManager.applyCollisionWithAlly(
        inst.x, inst.y, (float)ALLY_SPRITE_DIM, (float)ALLY_SPRITE_DIM,
        inst.speedX);
  }
  applyCollisionWithPlayer(px, py, pw, ph);
}

void AllyManager::run() {}

void AllyManager::destruct() {}

// Narrow hitbox matching the stick figure's actual body.
// Tweak kHitboxScaleX if horizontal feel is off.
static constexpr float kAllyHitboxScaleX = 0.25f;
static constexpr float kAllyHitboxScaleY = 1.0f;

static void allyHitbox(float instX, float instY,
                        float &ax, float &ay, float &aw, float &ah) {
  aw = (float)ALLY_SPRITE_DIM * kAllyHitboxScaleX;
  ah = (float)ALLY_SPRITE_DIM * kAllyHitboxScaleY;
  ax = instX + (float)ALLY_SPRITE_DIM * (1.f - kAllyHitboxScaleX) * 0.5f;
  ay = instY - 6.f;
}

bool AllyManager::playerRectBlocked(float px, float py, float pw, float ph) {
  for (Instance const &inst : instances) {
    if (!inst.active)
      continue;
    float ax, ay, aw, ah;
    allyHitbox(inst.x, inst.y, ax, ay, aw, ah);
    if (rectsOverlap(px, py, pw, ph, ax, ay, aw, ah))
      return true;
  }
  return false;
}

void AllyManager::applyCollisionWithPlayer(float px, float py, float pw,
                                           float ph) {
  constexpr float pad = 12.f;
  const float playerTop = py;

  for (Instance &inst : instances) {
    if (!inst.active)
      continue;
    float ax, ay, aw, ah;
    allyHitbox(inst.x, inst.y, ax, ay, aw, ah);
    if (!rectsOverlap(px, py, pw, ph, ax, ay, aw, ah))
      continue;
    const float allyFoot   = ay + ah;
    const float playerMidY = py + ph * 0.5f;
    const float allyMidY   = ay + ah * 0.5f;
    const bool topContact =
        allyMidY < playerMidY && allyFoot <= playerTop + pad;
    if (!topContact) {
      const bool allyRightOfPlayer = ax + aw * 0.5f > px + pw * 0.5f;
      const bool movingToward = allyRightOfPlayer ? inst.speedX < 0
                                                  : inst.speedX > 0;
      if (movingToward)
        inst.speedX = -inst.speedX;
    }
  }
}

bool AllyManager::cannonballOverlapsAlly(float cx, float cy, float cw,
                                         float ch) {
  for (Instance &inst : instances) {
    if (!inst.active)
      continue;
    if (rectsOverlap(cx, cy, cw, ch, inst.x, inst.y, (float)ALLY_SPRITE_DIM,
                     (float)ALLY_SPRITE_DIM))
      return true;
  }
  return false;
}

void AllyManager::render(SDL_Surface *surface) {
  if (gameState.replay.isReplayActive())
    return;
  for (const Instance &inst : instances) {
    if (!inst.active)
      continue;
    if (!gameState.isRectInVisionBox(inst.x, inst.y, (float)ALLY_SPRITE_DIM,
                                      (float)ALLY_SPRITE_DIM))
      continue;
    tex.draw(surface,
             {(int)std::floor(inst.x), (int)std::floor(inst.y),
              (uint16_t)ALLY_SPRITE_DIM, (uint16_t)ALLY_SPRITE_DIM},
             {255, 255, 255, 255});
  }
}
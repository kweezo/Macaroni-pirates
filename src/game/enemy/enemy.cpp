#include "enemy.hpp"

#include "enemy_tex_extern.hpp"
#include "enemy_inv_tex"
#include "enemy_tex"
#include "trash_tex_extern.hpp"
#include "trash_tex"

#include "game/ally/ally.hpp"
#include "manager/game_state.hpp"
#include "math/aabb.hpp"

#include <cmath>

EnemyManager::EnemyManager() : Process(5000) {}

void EnemyManager::init() {
  lastTime = NANOS;
  tex = Texture(enemyTexDat, enemyTexWidth, enemyTexHeight);
  invTex =
      Texture(enemyInvTexDat, enemyInvTexWidth, enemyInvTexHeight);
  trashTex = Texture(trashTexDat, trashTexWidth, trashTexHeight);
  if (!drawLinkedToMap) {
    addDependency((Drawable *)&gameState.map);
    drawLinkedToMap = true;
  }

  spawnStageWave();
}

void EnemyManager::run() {
  if (gameState.isGameOver())
    return;

  dt = (NANOS - lastTime) / (float)NS;
  lastTime = NANOS;

  for (Instance &instance : instances) {
    if (!instance.active)
      continue;
    updateInstance(instance);
  }

  gameState.allyManager.simulatePatrolAndAllyEnemyCollisions(dt);

  simulateFloatingTrash();

  tryAdvanceEnemyWaveLocked();
}

void EnemyManager::destruct() {}

void EnemyManager::render(SDL_Surface *surface) {
  if (gameState.replay.isReplayActive())
    return;

  for (TrashPiece const &tp : floatingTrash) {
    if (!tp.active)
      continue;
    if (!gameState.isRectInVisionBox(tp.x, tp.y, (float)TRASH_FLOAT_DIM,
                                      (float)TRASH_FLOAT_DIM))
      continue;
    trashTex.draw(surface,
                  {(uint16_t)std::floor(tp.x), (uint16_t)std::floor(tp.y),
                   TRASH_FLOAT_DIM, TRASH_FLOAT_DIM},
                  {255, 255, 255});
  }
  for (Instance &instance : instances) {
    if (!instance.active)
      continue;

    if (!gameState.isRectInVisionBox(instance.x, instance.y, (float)ENEMY_SIZE,
                                     (float)ENEMY_SIZE))
      continue;

    if (instance.invulnerable) {
      invTex.draw(
          surface,
          {(uint16_t)instance.x, (uint16_t)instance.y, ENEMY_SIZE, ENEMY_SIZE},
          {255, 200, 200});
    } else {
      tex.draw(surface,
               {(uint16_t)instance.x, (uint16_t)instance.y, ENEMY_SIZE,
                ENEMY_SIZE},
               {255, 255, 255});
    }
  }
}

bool EnemyManager::depositing(Instance const &instance) const {
  return NANOS - instance.oceanTouchTime < ENEMY_TRASH_DEPOSIT_TIME;
}

void EnemyManager::updateInstance(Instance &instance) {
  if (!depositing(instance))
    instance.lastTrashSpawnNano = 0;

  instance.invulnerable = false;

  if (depositing(instance)) {
    instance.invulnerable = true;
    if (!gameState.paused) {
      if (instance.lastTrashSpawnNano == 0) {
        spawnFloatingTrash(instance);
        instance.lastTrashSpawnNano = 1;
      }
    }
    return;
  }

  if (instance.dir) {
    instance.y += instance.speed * dt;
  } else {
    instance.y -= instance.speed * dt;
  }

  if (!gameState.map.onBeach(instance.x, instance.y) && !instance.dir) {
    instance.dir = true;
    instance.oceanTouchTime = NANOS;
  }

  if (instance.y > WINDOW_HEIGHT - ENEMY_SIZE)
    instance.dir = false;
}

void EnemyManager::applyCollisionWithAlly(float ax, float ay, float aw,
                                          float ah, float &allySpeedX) {
  constexpr float pad = 12.f;

  const float ew = (float)ENEMY_SIZE;
  const float eh = (float)ENEMY_SIZE;
  const float allyTop = ay;

  bool flipThisAllysPatrol = false;

  for (Instance &instance : instances) {
    if (!instance.active || depositing(instance))
      continue;
    if (!rectsOverlap(instance.x, instance.y, ew, eh, ax, ay, aw, ah))
      continue;

    const float enemyFoot = instance.y + eh;
    const float allyMidY = ay + ah * 0.5f;
    const float enemyMidY = instance.y + eh * 0.5f;

    const bool topContact =
        instance.dir ||
        (enemyMidY < allyMidY && enemyFoot <= allyTop + pad);

    if (topContact)
      instance.dir = false;
    else
      flipThisAllysPatrol = true;
  }

  if (flipThisAllysPatrol)
    allySpeedX = -allySpeedX;
}

void EnemyManager::spawnStageWave() {
  clearFloatingTrash();
  lastPlayerEnemyScorePenaltyNs = 0;
  for (Instance &instance : instances) {
    instance.active = false;
    instance.leftNeighbor = (uint32_t)-1;
    instance.rightNeighbor = (uint32_t)-1;
    instance.lastTrashSpawnNano = 0;
  }
  const StageEnemyWave cfg = gameState.enemyWaveConfig();
  spawnInstances(cfg.enemyCount, cfg.enemySpeedMultiplier);
}

void EnemyManager::tryAdvanceEnemyWaveLocked() {
  if (gameState.replay.isReplayActive())
    return;
  size_t alive = 0;
  for (Instance &instance : instances) {
    if (instance.active)
      ++alive;
  }
  if (alive != 0)
    return;
  gameState.onEnemyWaveCleared();
}

bool EnemyManager::cannonballOverlapsDepositingEnemy(float x, float y, float w,
                                                     float h) {
  const float ew = (float)ENEMY_SIZE;
  const float eh = (float)ENEMY_SIZE;
  for (Instance &instance : instances) {
    if (!instance.active || !depositing(instance))
      continue;
    if (rectsOverlap(x, y, w, h, instance.x, instance.y, ew, eh))
      return true;
  }
  return false;
}

int EnemyManager::hitEnemyByRect(float x, float y, float w, float h,
                                 int maxHits) {
  int left = maxHits;
  int hits = 0;
  for (Instance &instance : instances) {
    if (!instance.active || left <= 0 || depositing(instance))
      continue;
    if (rectsOverlap(x, y, w, h, instance.x, instance.y, (float)ENEMY_SIZE,
                     (float)ENEMY_SIZE)) {
      instance.active = false;
      ++hits;
      --left;
    }
  }
  return hits;
}

void EnemyManager::hitTrashByRect(float px, float py, float pw, float ph) {
  const float tw = (float)TRASH_FLOAT_DIM;
  const float th = (float)TRASH_FLOAT_DIM;
  for (TrashPiece &tp : floatingTrash) {
    if (!tp.active)
      continue;
    if (!rectsOverlap(px, py, pw, ph, tp.x, tp.y, tw, th))
      continue;
    tp.active = false;
    gameState.score += SCORE_POINTS_TRASH_COLLECT;
  }
}

void EnemyManager::applyPlayerEnemyScorePenalty(float px, float py, float pw,
                                                float ph) {
  const uint64_t now = static_cast<uint64_t>(NANOS);
  if (now - lastPlayerEnemyScorePenaltyNs <
      PLAYER_ENEMY_SCORE_PENALTY_COOLDOWN_NS)
    return;
  const float ew = (float)ENEMY_SIZE;
  const float eh = (float)ENEMY_SIZE;
  for (Instance const &instance : instances) {
    if (!instance.active)
      continue;
    if (!rectsOverlap(px, py, pw, ph, instance.x, instance.y, ew, eh))
      continue;
    lastPlayerEnemyScorePenaltyNs = now;
    gameState.subtractScoreBounded(SCORE_PENALTY_PLAYER_ENEMY_TOUCH);
    return;
  }
}

bool EnemyManager::cannonballDissolvesTrash(float x, float y, float w,
                                            float h) {
  const float tw = (float)TRASH_FLOAT_DIM;
  const float th = (float)TRASH_FLOAT_DIM;
  for (TrashPiece &tp : floatingTrash) {
    if (!tp.active)
      continue;
    if (!rectsOverlap(x, y, w, h, tp.x, tp.y, tw, th))
      continue;
    tp.active = false;
    return true;
  }
  return false;
}

void EnemyManager::spawnInstances(size_t count, float speedMultiplier) {
  assert(count <= MAX_ENEMY_COUNT);
  const uint32_t INTERVAL = WINDOW_WIDTH / MAX_ENEMY_COUNT;
  std::array<size_t, MAX_ENEMY_COUNT> filledSlots;
  filledSlots.fill(static_cast<size_t>(-1));

  uint32_t enemiesSpawned = 0;

  for (int x = 0; enemiesSpawned != count; x += (int)INTERVAL) {
    if (x >= (int)WINDOW_WIDTH)
      x = 0;
    const size_t slotIdx = (size_t)x / INTERVAL;
    if (rand() % MAX_ENEMY_COUNT || filledSlots[slotIdx] != static_cast<size_t>(-1))
      continue;

    Instance &sp = instances[enemiesSpawned];
    sp.x = (float)x;
    sp.y = WINDOW_HEIGHT - ENEMY_SIZE;
    sp.active = true;
    sp.speed = (BASE_ENEMY_SPEED + (rand() % 100 - 50) / 100.0f *
                                      ENEMY_SPEED_DEVIATION) *
               speedMultiplier;
    sp.oceanTouchTime = 0;
    sp.dir = false;
    sp.invulnerable = false;
    sp.lastTrashSpawnNano = 0;

    filledSlots[slotIdx] = enemiesSpawned;
    enemiesSpawned++;
  }

  for (Instance &instance : instances) {
    if (!instance.active)
      continue;

    const size_t slot = (size_t)instance.x / INTERVAL;
    if (slot != 0)
      instance.leftNeighbor = (uint32_t)filledSlots[slot - 1];
    if (slot < MAX_ENEMY_COUNT - 1)
      instance.rightNeighbor = (uint32_t)filledSlots[slot + 1];
  }
}

void EnemyManager::clearFloatingTrash() {
  for (TrashPiece &tp : floatingTrash)
    tp.active = false;
}

void EnemyManager::simulateFloatingTrash() {
  if (gameState.replay.isReplayActive())
    return;
  if (gameState.paused)
    return;
  constexpr float kMargin = 72.f;
  for (TrashPiece &tp : floatingTrash) {
    if (!tp.active)
      continue;
    tp.x += tp.vx * dt;
    tp.y += tp.vy * dt;
    if (tp.x < -kMargin || tp.y < -kMargin ||
        tp.x > (float)WINDOW_WIDTH + kMargin ||
        tp.y > (float)WINDOW_HEIGHT + kMargin)
      tp.active = false;
  }
}

void EnemyManager::spawnFloatingTrash(Instance const &instance) {
  TrashPiece *slot = nullptr;
  for (TrashPiece &tp : floatingTrash) {
    if (!tp.active) {
      slot = &tp;
      break;
    }
  }
  if (!slot)
    return;

  const unsigned gx = (unsigned)((int)(instance.x) >> 2);
  const unsigned gy = (unsigned)((int)(instance.y) >> 2);
  unsigned mix = gx ^ (gy * 2654435761u);
  mix ^= (unsigned)((instance.lastTrashSpawnNano >> 11) ^
                    (instance.lastTrashSpawnNano << 5));
  constexpr float kTrashSideFracMin = 0.35f;
  constexpr float kTrashSideFracSpan = 0.5f;
  const float sideFrac =
      kTrashSideFracMin +
      (float)(mix % 101u) / 100.f * kTrashSideFracSpan;
  const float sideSign = (mix & 1u) ? 1.f : -1.f;
  const float spd = TRASH_DRIFT_SPEED;
  const float vxMag = spd * sideFrac;
  slot->vx = sideSign * vxMag;
  slot->vy = -std::sqrt(std::fmax(0.f, spd * spd - vxMag * vxMag));
  slot->x = instance.x + 0.5f * ((float)ENEMY_SIZE - (float)TRASH_FLOAT_DIM);
  slot->y = instance.y + 0.5f * ((float)ENEMY_SIZE - (float)TRASH_FLOAT_DIM);
  slot->active = true;
}

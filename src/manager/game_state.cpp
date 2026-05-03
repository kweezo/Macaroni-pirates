#include "game_state.hpp"

#include "math/aabb.hpp"

GameState gameState = GameState();

GameState::GameState() : map(0.5) { scoreStore.reloadFromFile(); }

void GameState::reloadScoresFromDisk() { scoreStore.reloadFromFile(); }

void GameState::start(bool replayFromMenu) {
  replay.beginRound(replayFromMenu);
  score.store(0);
  renderer.initOnMainThread();
  processManager.start();
  processManager.addProcess(*(Process *)&renderer);
  processManager.addProcess(*(Process *)&player);
  processManager.addProcess(*(Process *)&enemyManager);
  processManager.addProcess(*(Process *)&allyManager);
  processManager.addProcess(*(Process *)&map);
}

float GameState::visionCenterX() const { return player.centerX(); }

float GameState::visionCenterY() const { return player.centerY(); }

bool GameState::isRectInVisionBox(float x, float y, float w, float h) const {
  const float cx = visionCenterX();
  const float cy = visionCenterY();
  const float vw = 2.0f * (float)VISION_RADIUS;
  const float vh = vw;
  const float vx = cx - (float)VISION_RADIUS;
  const float vy = cy - (float)VISION_RADIUS;
  return rectsOverlap(x, y, w, h, vx, vy, vw, vh);
}

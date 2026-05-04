#include "game_state.hpp"

#include "math/aabb.hpp"

constexpr uint64_t kEnemyStageDurationNs = 60ULL * NS;

static StageEnemyWave kEnemyStages[GameState::kEnemyStageCount] = {
    {0.50f, 10, 1.0f},
    {0.42f, 12, 1.10f},
    {0.34f, 14, 1.22f},
};

GameState gameState = GameState();

GameState::GameState() : map(0.5) { scoreStore.reloadFromFile(); }

void GameState::reloadScoresFromDisk() { scoreStore.reloadFromFile(); }

void GameState::shutdownFromUser() {
  replay.flushRecording();
  gameRunning = false;
  processManager.stop();
}

bool GameState::isGameOver() const {
  return gameOver;
}

void GameState::triggerGameOver() {
  if (gameOver)
    return;
  gameOver = true;
  paused = true;
}

void GameState::restartEnemyStageClock() {
  enemyStageStartedNs = static_cast<uint64_t>(NANOS);
}

bool GameState::enemyStageTimeExpired() const {
  if (replay.isReplayActive())
    return false;
  const uint64_t now = static_cast<uint64_t>(NANOS);
  if (now - enemyStageStartedNs >= kEnemyStageDurationNs)
    return true;
  return false;
}

int GameState::enemyStageSecondsRemainingForHud() const {
  if (replay.isReplayActive())
    return -1;
  const uint64_t now = static_cast<uint64_t>(NANOS);
  if (now < enemyStageStartedNs)
    return 60;
  const uint64_t elapsed = now - enemyStageStartedNs;
  if (elapsed >= kEnemyStageDurationNs)
    return 0;
  return static_cast<int>((kEnemyStageDurationNs - elapsed) / NS);
}

void GameState::start(bool replayFromMenu) {
  paused = false;
  gameOver = false;
  gameRunning = true;
  replay.beginRound(replayFromMenu);
  score = 0;
  enemyWaveIndex = 0;
  map.setBeachRatio(enemyWaveConfig().beachRatio);
  player.prepareForNewRound();
  if (!replay.isReplayActive())
    player.placeRandomTopSpawn();
  restartEnemyStageClock();
  renderer.initOnMainThread();
  processManager.start();
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

StageEnemyWave GameState::enemyWaveConfig() const {
  return kEnemyStages[enemyWaveIndex];
}

int GameState::stageNumberOneBased() const {
  return enemyWaveIndex + 1;
}

void GameState::subtractScoreBounded(int amount) {
  if (amount <= 0)
    return;
  score = score > amount ? score - amount : 0;
}

void GameState::onEnemyWaveCleared() {
  if (isGameOver())
    return;
  if (replay.isReplayActive())
    return;
  if (enemyWaveIndex >= kEnemyStageCount - 1)
    return;
  ++enemyWaveIndex;
  map.setBeachRatio(enemyWaveConfig().beachRatio);
  enemyManager.spawnStageWave();
  player.placeRandomTopSpawn();
  restartEnemyStageClock();
}

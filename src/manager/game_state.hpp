#pragma once

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <mutex>

#include "game/ally/ally.hpp"
#include "game/enemy/enemy.hpp"
#include "game/map/map.hpp"
#include "game/player/player.hpp"
#include "game/player/player_replay.hpp"
#include "game/player_profile.hpp"
#include "game/score_store.hpp"
#include "manager/process/process_manager.hpp"
#include "renderer/renderer.hpp"

#include "macros.hpp"

struct StageEnemyWave {
  float beachRatio;
  std::size_t enemyCount;
  float enemySpeedMultiplier;
};

struct GameState {
  GameState();
  void start(bool replayFromMenu);
  float visionCenterX() const;
  float visionCenterY() const;
  bool isRectInVisionBox(float x, float y, float w, float h) const;

  static constexpr int kEnemyStageCount = 3;

  StageEnemyWave enemyWaveConfig() const;
  int stageNumberOneBased() const;
  void subtractScoreBounded(int amount);
  void onEnemyWaveCleared();

  EnemyManager enemyManager;
  AllyManager allyManager;
  ScoreStore scoreStore;
  ProcessManager processManager;
  Renderer renderer;
  Player player;
  Map map;

  PlayerReplay replay;
  PlayerProfile playerProfile;

  static constexpr int VISION_RADIUS = 220;

  std::atomic<int> score{};
  std::atomic<bool> paused{false};
  std::atomic<bool> gameOver{false};
  std::atomic<bool> gameRunning{false};

  std::recursive_mutex worldSimMutex;

  void reloadScoresFromDisk();
  void shutdownFromUser();
  bool isGameOver() const;
  void triggerGameOver();
  void restartEnemyStageClock();
  bool enemyStageTimeExpired() const;
  int enemyStageSecondsRemainingForHud() const;

private:
  int enemyWaveIndex = 0;
  uint64_t enemyStageStartedNs = 0;
};

extern GameState gameState;

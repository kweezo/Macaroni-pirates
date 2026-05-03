#pragma once

#include <atomic>
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

struct GameState {
  GameState();
  void start(bool replayFromMenu);
  float visionCenterX() const;
  float visionCenterY() const;
  bool isRectInVisionBox(float x, float y, float w, float h) const;

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

  std::mutex worldSimMutex;

  void reloadScoresFromDisk();
};

extern GameState gameState;

#pragma once

#include <cstdint>
#include <atomic>

#include "manager/process/process_manager.hpp"
#include "renderer/renderer.hpp"
#include "game/player/player.hpp"
#include "game/map/map.hpp"
#include "game/enemy/enemy.hpp"

#include "macros.hpp"

struct GameState {
    GameState();
    void start();

    EnemyManager enemyManager;
    ProcessManager processManager;
    Renderer renderer;
    Player player;
    Map map;
};

extern GameState gameState;
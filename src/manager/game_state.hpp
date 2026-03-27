#pragma once

#include <cstdint>
#include <atomic>

#include "manager/process/process_manager.hpp"
#include "renderer/renderer.hpp"
#include "game/player/player.hpp"
#include "game/map/map.hpp"

#include "macros.hpp"

struct GameState {
    GameState();
    void start();

    ProcessManager processManager;
    Renderer renderer;
    Player player;
    Map map;
};

extern GameState gameState;
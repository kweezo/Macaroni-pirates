#pragma once

#include <cstdint>
#include <atomic>

#include "manager/process/process_manager.hpp"
#include "renderer/renderer.hpp"
#include "game/player/player.hpp"

//todo move into separate header
#define MILLIS std::chrono::system_clock::now().time_since_epoch().count() 
#define NS 1000000

struct GameState {
    GameState();
    void start();

    ProcessManager processManager;
    Renderer renderer;
    Player player;
};

extern GameState gameState;
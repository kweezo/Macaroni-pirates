#pragma once

#include <cstdint>
#include <atomic>

#include "manager/process/process_manager.hpp"
#include "renderer/renderer.hpp"
#include "game/player/player.hpp"
#include "game/map/map.hpp"

//todo move into separate header
#define MILLIS std::chrono::system_clock::now().time_since_epoch().count() 
#define NS 1000000

#define WINDOW_WIDTH 1600
#define WINDOW_HEIGHT 900

struct GameState {
    GameState();
    void start();

    ProcessManager processManager;
    Renderer renderer;
    Player player;
    Map map;
};

extern GameState gameState;
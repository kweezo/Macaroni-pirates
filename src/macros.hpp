#pragma once

#define NANOS std::chrono::system_clock::now().time_since_epoch().count()
#define NS 1000000000

#define WINDOW_WIDTH 1600
#define WINDOW_HEIGHT 900

// #define MACARONI_DEBUG_QUICK_STAGE

#ifdef MACARONI_DEBUG_QUICK_STAGE
#define SCORE_POINTS_ENEMY_KILL 500
#else
#define SCORE_POINTS_ENEMY_KILL 50
#endif

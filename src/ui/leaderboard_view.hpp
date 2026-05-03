#pragma once

#include "game/score_store.hpp"

struct SDL_Surface;

namespace LeaderboardView {

void drawMainMenuStrip(SDL_Surface *surface, const ScoreStore &store);

void drawPauseStrip(SDL_Surface *surface, const ScoreStore &store);

void drawTopN(SDL_Surface *surface, const ScoreStore &store, float titleX,
              float titleY, float listX, float listY, float titleScale,
              float rowScale);

}

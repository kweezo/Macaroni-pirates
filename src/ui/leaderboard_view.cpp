#include "ui/leaderboard_view.hpp"

#include "renderer/font/font_renderer.hpp"
#include "macros.hpp"

#include <SDL3/SDL.h>

#include <algorithm>
#include <cstdio>

namespace LeaderboardView {

namespace {

constexpr float kRowStepY = 28.0f;

}

void drawTopN(SDL_Surface *surface, const ScoreStore &store, float titleX,
              float titleY, float listX, float listY, float titleScale,
              float rowScale) {
  if (!surface)
    return;

  FontRenderer::drawText(surface, titleX, titleY, titleScale, "Top 5", 230, 220,
                         200,
                         std::max(40.f, (float)WINDOW_WIDTH - titleX - 16.f));

  ScoreStore::Entry rows[SCORE_STORE_LEADERBOARD_DISPLAY_SLOTS]{};
  store.topForDisplay(rows);

  float rowY = listY;
  const float rowClipW =
      std::max(40.f, (float)WINDOW_WIDTH - listX - 16.f);
  for (size_t i = 0; i < SCORE_STORE_LEADERBOARD_DISPLAY_SLOTS; ++i) {
    const ScoreStore::Entry &ent = rows[i];
    if (!ent.name[0])
      continue;
    char line[96];
    std::snprintf(line, sizeof line, "%zu. %s  %d", i + 1, ent.name, ent.score);
    FontRenderer::drawText(surface, listX, rowY, rowScale, line, 210, 210, 220,
                           rowClipW);
    rowY += kRowStepY;
  }
}

void drawMainMenuStrip(SDL_Surface *surface, const ScoreStore &store) {
  drawTopN(surface, store, 920.0f, 36.0f, 920.0f, 80.0f, 0.34f, 0.26f);
}

void drawPauseStrip(SDL_Surface *surface, const ScoreStore &store) {
  drawTopN(surface, store, 920.0f, 360.0f, 920.0f, 404.0f, 0.28f, 0.22f);
}

}

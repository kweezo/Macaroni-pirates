#include "ui/game_ui.hpp"

#include "manager/game_state.hpp"
#include "macros.hpp"
#include "renderer/font/font_renderer.hpp"

#include <SDL3/SDL.h>

#include <cstdio>

void GameUi::layout() {
  constexpr float margin = 16.0f;
  constexpr float btnW = 160.0f;
  constexpr float btnH = 44.0f;
  quitButton = UIButton((float)WINDOW_WIDTH - btnW - margin, margin, btnW,
                        btnH);
  constexpr float goW = 300.0f;
  constexpr float goH = 52.0f;
  gameOverMenuButton = UIButton((float)WINDOW_WIDTH * 0.5f - goW * 0.5f,
                                (float)WINDOW_HEIGHT * 0.5f + 100.0f, goW,
                                goH);
  pauseMenu.layout();
}

void GameUi::draw(SDL_Surface *surface, GameState &gs) {
  if (!surface)
    return;

  if (gs.isGameOver()) {
    const SDL_PixelFormatDetails *fmt =
        SDL_GetPixelFormatDetails(surface->format);
    if (fmt)
      SDL_FillSurfaceRect(
          surface, nullptr,
          SDL_MapRGBA(fmt, nullptr, 10, 14, 28, (uint8_t)245));

    constexpr float titleScale = 0.62f;
    const char *title = "GAME OVER";
    const float tw = FontRenderer::estimateTextWidth(title, titleScale);
    FontRenderer::drawText(
        surface, (float)surface->w * 0.5f - tw * 0.5f, 200.0f, titleScale,
        title, 255, 210, 180, (float)surface->w - 48.0f);

    char stats[144];
    const int sc = gs.score.load(std::memory_order_relaxed);
    std::snprintf(stats, sizeof stats, "Score %d stage %d/%d. Main menu.", sc,
                  gs.stageNumberOneBased(), GameState::kEnemyStageCount);
    FontRenderer::drawText(surface, 72.0f, 296.0f, 0.30f, stats, 200, 210, 228,
                           (float)surface->w - 144.0f);

    gameOverMenuButton.draw(surface, "Main menu", 0.31f);
    return;
  }

  if (gs.paused.load(std::memory_order_relaxed)) {
    pauseMenu.draw(surface, gs);
    return;
  }

  char line[128];
  const int sc = gs.score.load(std::memory_order_relaxed);
  const int st = gs.stageNumberOneBased();
  const int hp = gs.player.health();
  constexpr float hudQuitReserve = 160.0f + 16.0f + 32.0f;
  const float hudMaxW = (float)surface->w - hudQuitReserve;
  std::snprintf(line, sizeof line, "Stage %d/%d  score %d  HP %d", st,
                GameState::kEnemyStageCount, sc, hp);
  FontRenderer::drawText(surface, 12.0f, 12.0f, 0.36f, line, 230, 240, 250,
                         hudMaxW);

  const int secLeft = gs.enemyStageSecondsRemainingForHud();
  if (secLeft >= 0) {
    char clock[16];
    const int m = secLeft / 60;
    const int s = secLeft % 60;
    std::snprintf(clock, sizeof clock, "%d:%02d", m, s);
    constexpr float clockScale = 0.52f;
    FontRenderer::drawText(surface, 12.0f, 52.0f, clockScale, clock, 255, 215,
                           140, (float)surface->w);
  }

  quitButton.draw(surface, "Quit", 0.28f);
}

void GameUi::handleClick(float mx, float my, GameState &gs) {
  if (gs.isGameOver()) {
    if (gameOverMenuButton.hitTest(mx, my))
      gs.shutdownFromUser();
    return;
  }

  if (gs.paused.load(std::memory_order_relaxed)) {
    pauseMenu.handleClick(mx, my, gs);
    return;
  }
  if (quitButton.hitTest(mx, my)) {
    gs.shutdownFromUser();
  }
}

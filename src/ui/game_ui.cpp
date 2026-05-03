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
  pauseMenu.layout();
}

void GameUi::draw(SDL_Surface *surface, GameState &gs) {
  if (!surface)
    return;

  if (gs.paused.load(std::memory_order_relaxed)) {
    pauseMenu.draw(surface, gs);
    return;
  }

  char line[64];
  const int sc = gs.score.load(std::memory_order_relaxed);
  std::snprintf(line, sizeof line, "Score %d", sc);
  FontRenderer::drawText(surface, 12.0f, 12.0f, 0.36f, line, 230, 240, 250);

  quitButton.draw(surface, "Quit", 0.28f);
}

void GameUi::handleClick(float mx, float my, GameState &gs) {
  if (gs.paused.load(std::memory_order_relaxed)) {
    pauseMenu.handleClick(mx, my, gs);
    return;
  }
  if (quitButton.hitTest(mx, my)) {
    gs.replay.flushRecording();
    gs.processManager.stop();
  }
}

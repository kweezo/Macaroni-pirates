#include "ui/pause_menu.hpp"

#include "macros.hpp"
#include "manager/game_state.hpp"
#include "renderer/font/font_renderer.hpp"
#include "ui/leaderboard_view.hpp"

#include <SDL3/SDL.h>

void PauseMenu::layout() {
  const float btnW = 220.0f;
  const float btnH = 48.0f;
  const float cx = (float)WINDOW_WIDTH * 0.5f - btnW * 0.5f;
  const float midY = (float)WINDOW_HEIGHT * 0.5f;
  resumeButton = UIButton(cx, midY - btnH - 12.0f, btnW, btnH);
  saveScoreButton = UIButton(cx, midY, btnW, btnH);
  quitButton = UIButton(cx, midY + btnH + 12.0f, btnW, btnH);
}

void PauseMenu::draw(SDL_Surface *surface, GameState &gs) const {
  if (!surface)
    return;
  constexpr float pauseScale = 0.48f;
  const char *pauseText = "Paused";
  const float pauseW = FontRenderer::estimateTextWidth(pauseText, pauseScale);
  FontRenderer::drawText(surface, (float)WINDOW_WIDTH * 0.5f - pauseW * 0.5f,
                         120.0f, pauseScale, pauseText, 255, 220, 160);
  resumeButton.draw(surface, "Resume", 0.3f);
  saveScoreButton.draw(surface, "Save score", 0.3f);
  quitButton.draw(surface, "Quit to OS", 0.3f);

  LeaderboardView::drawPauseStrip(surface, gs.scoreStore);
}

void PauseMenu::handleClick(float mx, float my, GameState &gs) {
  if (resumeButton.hitTest(mx, my)) {
    gs.paused.store(false, std::memory_order_relaxed);
    return;
  }
  if (saveScoreButton.hitTest(mx, my)) {
    const int sc = gs.score.load(std::memory_order_relaxed);
    gs.scoreStore.addEntry(gs.playerProfile.nameUtf8(), sc);
    return;
  }
  if (quitButton.hitTest(mx, my)) {
    gs.shutdownFromUser();
  }
}

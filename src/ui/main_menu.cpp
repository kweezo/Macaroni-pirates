#include "ui/main_menu.hpp"

#include "macros.hpp"
#include "manager/game_state.hpp"
#include "renderer/font/font_renderer.hpp"
#include "ui/leaderboard_view.hpp"

#include <SDL3/SDL.h>

MainMenu::~MainMenu() {
  if (window)
    SDL_StopTextInput(window);
  if (window) {
    SDL_DestroyWindow(window);
    window = nullptr;
  }
}

MainMenuResult MainMenu::run(GameState &gs) {
  if (!SDL_Init(SDL_INIT_VIDEO))
    return MainMenuResult::Error;

  window = SDL_CreateWindow("Macaroni Pirates", WINDOW_WIDTH, WINDOW_HEIGHT, 0);
  if (!window) {
    SDL_Quit();
    return MainMenuResult::Error;
  }

  nameField.setWindow(window);
  nameField.setRect(48.0f, 218.0f, 420.0f, 48.0f);
  nameField.setPlaceholder("Your name");
  nameField.setText(gs.playerProfile.nameUtf8());

  gs.reloadScoresFromDisk();

  const float btnW = 240.0f;
  const float btnH = 48.0f;
  const float leftX = 48.0f;
  const float playY = 300.0f;
  playButton = UIButton(leftX, playY, btnW, btnH);
  replayButton = UIButton(leftX, playY + btnH + 12.0f, btnW, btnH);
  quitButton = UIButton(leftX, playY + 2.0f * btnH + 28.0f, btnW, btnH);

  done = false;
  outcome = MainMenuResult::Quit;

  while (!done) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_EVENT_QUIT) {
        done = true;
      } else if (nameField.handleSDLEvent(e)) {
        continue;
      } else if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN && e.button.down &&
                 e.button.button == SDL_BUTTON_LEFT) {
        const float mx = (float)e.button.x;
        const float my = (float)e.button.y;
        if (nameField.handleMouseDown(mx, my))
          continue;
        handleClick(mx, my, gs);
      }
    }
    drawFrame(gs);
    SDL_UpdateWindowSurface(window);
    SDL_Delay(16);
  }

  if (window)
    SDL_StopTextInput(window);
  if (window) {
    SDL_DestroyWindow(window);
    window = nullptr;
  }

  const bool keepVideoForGame =
      outcome == MainMenuResult::Play ||
      outcome == MainMenuResult::ReplayLast;
  if (!keepVideoForGame && SDL_WasInit(SDL_INIT_VIDEO))
    SDL_Quit();

  return outcome;
}

void MainMenu::applyOptionalName(GameState &gs) {
  const char *nm = nameField.text();
  if (nm[0])
    gs.playerProfile.setFromUtf8(nm);
}

void MainMenu::handleClick(float mx, float my, GameState &gs) {
  if (playButton.hitTest(mx, my)) {
    applyOptionalName(gs);
    outcome = MainMenuResult::Play;
    done = true;
    return;
  }
  if (replayButton.hitTest(mx, my)) {
    applyOptionalName(gs);
    outcome = MainMenuResult::ReplayLast;
    done = true;
    return;
  }
  if (quitButton.hitTest(mx, my)) {
    outcome = MainMenuResult::Quit;
    done = true;
  }
}

void MainMenu::drawFrame(GameState &gs) {
  SDL_Surface *surface = SDL_GetWindowSurface(window);
  if (!surface)
    return;

  const SDL_PixelFormatDetails *fmt =
      SDL_GetPixelFormatDetails(surface->format);
  if (!fmt)
    return;

  const uint32_t bg = SDL_MapRGBA(fmt, nullptr, 24, 32, 48, 255);
  SDL_FillSurfaceRect(surface, nullptr, bg);

  constexpr float kMenuTextMaxW = 920.f - 48.f - 24.f;

  FontRenderer::drawText(surface, 48.0f, 28.0f, 0.52f, "Macaroni Pirates", 240,
                         220, 160, kMenuTextMaxW);
  FontRenderer::drawText(surface, 48.0f, 88.0f, 0.34f, "Your name", 230, 228,
                         210, kMenuTextMaxW);

  FontRenderer::drawText(surface, 48.0f, 124.0f, 0.24f,
                         "Optional. Shown next to scores\nwhen you play.", 170,
                         175, 195, kMenuTextMaxW);

  nameField.draw(surface, 0.28f);
  playButton.draw(surface, "Play", 0.28f);
  replayButton.draw(surface, "Replay", 0.28f);
  quitButton.draw(surface, "Quit", 0.28f);

  LeaderboardView::drawMainMenuStrip(surface, gs.scoreStore);
}

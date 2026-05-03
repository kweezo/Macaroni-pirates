#include "ui/main_menu.hpp"

#include "macros.hpp"
#include "manager/game_state.hpp"
#include "renderer/font/font_renderer.hpp"

#include <SDL3/SDL.h>

#include <cstring>

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
  nameField.setRect(48.0f, 200.0f, 360.0f, 44.0f);

  const float btnW = 220.0f;
  const float btnH = 48.0f;
  const float leftX = 48.0f;
  const float playY = 280.0f;
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

void MainMenu::handleClick(float mx, float my, GameState &gs) {
  if (playButton.hitTest(mx, my)) {
    const char *nm = nameField.text();
    if (nm[0])
      gs.playerProfile.setFromUtf8(nm);
    outcome = MainMenuResult::Play;
    done = true;
    return;
  }
  if (replayButton.hitTest(mx, my)) {
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

  gs.reloadScoresFromDisk();

  const SDL_PixelFormatDetails *fmt =
      SDL_GetPixelFormatDetails(surface->format);
  if (!fmt)
    return;

  const uint32_t bg = SDL_MapRGBA(fmt, nullptr, 24, 32, 48, 255);
  SDL_FillSurfaceRect(surface, nullptr, bg);

  FontRenderer::drawText(surface, 48.0f, 28.0f, 0.52f, "Macaroni Pirates", 240,
                         220, 160);
  FontRenderer::drawText(surface, 48.0f, 88.0f, 0.28f, "Name (for scoreboard)",
                         200, 200, 210);

  nameField.draw(surface, 0.28f);
  playButton.draw(surface, "Play", 0.28f);
  replayButton.draw(surface, "Replay last", 0.28f);
  quitButton.draw(surface, "Quit", 0.28f);

  FontRenderer::drawText(surface, 920.0f, 36.0f, 0.34f, "Top 5", 230, 220, 200);

  const float listX = 920.0f;
  float listY = 80.0f;
  ScoreStore::Entry rows[SCORE_STORE_LEADERBOARD_DISPLAY_SLOTS]{};
  gs.scoreStore.topForDisplay(rows);
  for (size_t i = 0; i < SCORE_STORE_LEADERBOARD_DISPLAY_SLOTS; ++i) {
    const ScoreStore::Entry &ent = rows[i];
    if (!ent.name[0])
      continue;
    char line[80];
    std::snprintf(line, sizeof line, "%zu. %s  %d", i + 1, ent.name, ent.score);
    FontRenderer::drawText(surface, listX, listY, 0.26f, line, 210, 210, 220);
    listY += 28.0f;
  }
}

#include "manager/game_state.hpp"
#include "ui/main_menu.hpp"

#include <SDL3/SDL.h>

int main() {
  MainMenu menu;
  switch (menu.run(gameState)) {
  case MainMenuResult::Error:
  case MainMenuResult::Quit:
    return 0;
  case MainMenuResult::Play:
    if (!SDL_WasInit(SDL_INIT_VIDEO) && !SDL_Init(SDL_INIT_VIDEO))
      return 1;
    gameState.start(false);
    break;
  case MainMenuResult::ReplayLast:
    if (!SDL_WasInit(SDL_INIT_VIDEO) && !SDL_Init(SDL_INIT_VIDEO))
      return 1;
    gameState.start(true);
    break;
  }

  gameState.processManager.joinWorkers();
  gameState.renderer.shutdownVideoOnMain();

  if (SDL_WasInit(SDL_INIT_VIDEO))
    SDL_Quit();
  return 0;
}

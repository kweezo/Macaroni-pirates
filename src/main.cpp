#include "manager/game_state.hpp"
#include "ui/main_menu.hpp"

#include <SDL3/SDL.h>

int main() {
  for (;;) {
    MainMenu menu;
    const MainMenuResult pick = menu.run(gameState);

    if (pick == MainMenuResult::Error || pick == MainMenuResult::Quit) {
      if (SDL_WasInit(SDL_INIT_VIDEO))
        SDL_Quit();
      return 0;
    }

    if (!SDL_WasInit(SDL_INIT_VIDEO) && !SDL_Init(SDL_INIT_VIDEO))
      return 1;

    gameState.start(pick == MainMenuResult::ReplayLast);

    while (gameState.gameRunning) {
      gameState.renderer.frame();
    }

    gameState.processManager.joinWorkers();
    gameState.renderer.shutdownVideoOnMain();
  }
}

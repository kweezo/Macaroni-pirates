#pragma once

#include "ui/pause_menu.hpp"
#include "ui/ui_button.hpp"

struct SDL_Surface;
struct GameState;

class GameUi {
public:
  void layout();
  void draw(SDL_Surface *surface, GameState &gs);
  void handleClick(float mx, float my, GameState &gs);

private:
  UIButton quitButton{};
  PauseMenu pauseMenu{};
};

#pragma once

#include "ui/ui_button.hpp"

struct SDL_Surface;
struct GameState;

class PauseMenu {
public:
  void layout();
  void draw(SDL_Surface *surface, GameState &gs) const;
  void handleClick(float mx, float my, GameState &gs);

private:
  UIButton resumeButton{};
  UIButton saveScoreButton{};
  UIButton quitButton{};
};

#pragma once

#include <cstdio>

#include "ui/ui_button.hpp"
#include "ui/ui_text_field.hpp"

struct SDL_Window;
struct GameState;

enum class MainMenuResult { Error, Quit, Play, ReplayLast };

class MainMenu {
public:
  MainMenu() = default;
  ~MainMenu();

  MainMenu(const MainMenu &) = delete;
  MainMenu &operator=(const MainMenu &) = delete;

  MainMenuResult run(GameState &gs);

private:
  void drawFrame(GameState &gs);
  void handleClick(float mx, float my, GameState &gs);

  SDL_Window *window = nullptr;
  UITextField nameField{};
  UIButton playButton{};
  UIButton replayButton{};
  UIButton quitButton{};
  bool done = false;
  MainMenuResult outcome = MainMenuResult::Quit;
};

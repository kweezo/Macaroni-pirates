#pragma once

#include <SDL3/SDL.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <functional>
#include <future>
#include <mutex>
#include <stdexcept>

#include "manager/process/process.hpp"
#include "ui/game_ui.hpp"

#define DRAW_STACK_SIZE 64

class Drawable; // fucking disgusting I have to do this but necessary

class Renderer : public Process {
  void init() override;
  void run() override;
  void destruct() override;

public:
  Renderer();

  void initOnMainThread();
  void shutdownVideoOnMain();

  size_t getNextFreeDrawIndex(Drawable *drawable);
  void removeDrawIndex(size_t i);

  SDL_Surface *getWindowSurface();

private:
  void createWindow();
  void createRenderer();

  void render();
  void applyInvertedCircleMask(SDL_Surface *surface);
  void dispatchUiClick(float mx, float my);

  SDL_Window *window = nullptr;
  SDL_Renderer *renderer = nullptr;

  std::array<Drawable *, DRAW_STACK_SIZE> drawStack;
  std::array<std::future<void>, DRAW_STACK_SIZE> drawFutures;
  std::mutex drawStackMutex;

  GameUi gameUi;
};

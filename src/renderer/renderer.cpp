#include "renderer.hpp"

#include "renderer/circle_mask_lookup.hpp"

#include "macros.hpp"
#include "manager/game_state.hpp"

using VisionCircleMask = CircleMaskLookup<GameState::VISION_RADIUS>;

Renderer::Renderer() {}

void Renderer::initOnMainThread() {
  createWindow();
  createRenderer();
  gameUi.layout();

  SDL_Event e;
  while (SDL_PollEvent(&e)) {
  }
}

void Renderer::shutdownVideoOnMain() {
  if (renderer) {
    SDL_DestroyRenderer(renderer);
    renderer = nullptr;
  }
  if (window) {
    SDL_DestroyWindow(window);
    window = nullptr;
  }
}

void Renderer::frame() {
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    if (e.type == SDL_EVENT_QUIT) {
      gameState.shutdownFromUser();
    } else if (e.type == SDL_EVENT_KEY_DOWN && e.key.down && !e.key.repeat &&
               e.key.key == SDLK_ESCAPE) {
      if (!gameState.isGameOver()) {
        const bool p = gameState.paused;
        gameState.paused = !p;
      }
    } else if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN && e.button.down &&
               e.button.button == SDL_BUTTON_LEFT) {
      dispatchUiClick(e.button.x, e.button.y);
    }
  }

  SDL_Surface *const surf = getWindowSurface();
  const SDL_PixelFormatDetails *formatDetails =
      SDL_GetPixelFormatDetails(surf->format);
  const uint32_t blueColor =
      SDL_MapRGBA(formatDetails, nullptr, 96, 140, 220, 255);
  if (gameState.replay.isReplayActive())
    SDL_FillSurfaceRect(surf, nullptr, blueColor);

  const int centerX = (int)std::floor(gameState.visionCenterX());
  const int centerY = (int)std::floor(gameState.visionCenterY());
  constexpr int vr = GameState::VISION_RADIUS;
  const int rectLeft = std::max(0, centerX - vr);
  const int rectTop = std::max(0, centerY - vr);
  const int rectRight = std::min(surf->w, centerX + vr);
  const int rectBottom = std::min(surf->h, centerY + vr);
  for (int y = rectTop; y < rectBottom; ++y) {
    uint32_t *row =
        (uint32_t *)((uint8_t *)surf->pixels + y * surf->pitch);
    for (int x = rectLeft; x < rectRight; ++x) {
      row[x] = blueColor;
    }
  }

  render();
  applyInvertedCircleMask(surf);

  gameUi.draw(surf, gameState);

  SDL_UpdateWindowSurface(window);
}

void Renderer::dispatchUiClick(float mx, float my) {
  gameUi.handleClick(mx, my, gameState);
}

void Renderer::createWindow() {
  window = SDL_CreateWindow("Macaroni Pirates", WINDOW_WIDTH, WINDOW_HEIGHT, 0);

  if (!window)
    throw std::runtime_error("Failed to create window");
  SDL_ShowWindow(window);
  SDL_RaiseWindow(window);
}

void Renderer::createRenderer() {

  renderer = SDL_CreateSoftwareRenderer(getWindowSurface());
  if (!renderer)
    throw std::runtime_error("Failed to create renderer");
}

void Renderer::render() {
  std::array<Drawable *, DRAW_STACK_SIZE> drawStackCopy;

  drawStackMutex.lock();
  std::copy(drawStack.begin(), drawStack.end(), drawStackCopy.begin());
  drawStackMutex.unlock();

  for (int i = 0; i < DRAW_STACK_SIZE; i++) {
    if (drawStack[i])
      drawFutures[i] = std::async(&Drawable::renderWrapper, drawStackCopy[i],
                                  getWindowSurface());
  }

  for (int i = 0; i < DRAW_STACK_SIZE; i++) {
    if (drawFutures[i].valid())
      drawFutures[i].get();
  }
  for (int i = 0; i < DRAW_STACK_SIZE; i++) {
    if (drawStack[i])
      drawStack[i]->resetMutex();
  }
}

void Renderer::applyInvertedCircleMask(SDL_Surface *surface) {
  constexpr int radius = GameState::VISION_RADIUS;
  constexpr auto minXPerRow = VisionCircleMask::MinXPerRow;
  constexpr auto maxXPerRow = VisionCircleMask::MaxXPerRow;
  const int centerX = (int)std::floor(gameState.visionCenterX());
  const int centerY = (int)std::floor(gameState.visionCenterY());
  const SDL_PixelFormatDetails *formatDetails =
      SDL_GetPixelFormatDetails(surface->format);
  const uint32_t fogColor =
      SDL_MapRGBA(formatDetails, nullptr, 236, 236, 242, 255);

  for (int y = 0; y < surface->h; ++y) {
    uint32_t *row =
        (uint32_t *)((uint8_t *)surface->pixels + y * surface->pitch);
    const int dy = y - centerY;
    if (dy < -radius || dy > radius) {
      for (int x = 0; x < surface->w; ++x) {
        row[x] = fogColor;
      }
      continue;
    }

    const int rowIndex = dy + radius;
    const int visibleMinX = centerX + minXPerRow[rowIndex];
    const int visibleMaxX = centerX + maxXPerRow[rowIndex];
    if (visibleMaxX < 0 || visibleMinX >= surface->w) {
      for (int x = 0; x < surface->w; ++x) {
        row[x] = fogColor;
      }
      continue;
    }

    const int leftEnd = std::min(surface->w - 1, visibleMinX - 1);
    for (int x = 0; x <= leftEnd; ++x) {
      row[x] = fogColor;
    }

    const int rightStart = std::max(0, visibleMaxX + 1);
    for (int x = rightStart; x < surface->w; ++x) {
      row[x] = fogColor;
    }
  }
}

size_t Renderer::getNextFreeDrawIndex(Drawable *drawable) {
  std::lock_guard lock(drawStackMutex);
  for (int i = 0; i < DRAW_STACK_SIZE; i++) {
    if (drawStack[i])
      continue;

    drawStack[i] = drawable;
    return i;
  }

  throw std::runtime_error("Out of free render slots");
}

void Renderer::removeDrawIndex(size_t i) {
  std::lock_guard lock(drawStackMutex);
  drawStack[i] = {};
}

SDL_Surface *Renderer::getWindowSurface() {
  return SDL_GetWindowSurface(window);
}

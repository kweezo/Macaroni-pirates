#include "map.hpp"

#include "manager/game_state.hpp"
#include "macros.hpp"

#include "sand_tex"

#include <algorithm>
#include <array>
#include <cmath>
#include <future>

namespace {

constexpr int kSandTile = 32;
constexpr size_t kSandRowFutures =
    (WINDOW_HEIGHT + kSandTile - 1) / kSandTile;

}

Map::Map() = default;

Map::Map(float r) : beachRatio(r) {}

void Map::setBeachRatio(float ratio) { beachRatio = ratio; }

void Map::init() {
  sandTex = Texture(sandTexDat, sandTexWidth, sandTexHeight);
}

void Map::run() {}

void Map::destruct() {}

void Map::render(SDL_Surface *surface) {
  std::array<std::future<void>, kSandRowFutures> futures{};

  const uint16_t sandTop = static_cast<uint16_t>(
      std::floor(beachRatio * (float)WINDOW_HEIGHT));

  const float vr = (float)GameState::VISION_RADIUS;
  const int clipMinX =
      std::max(0, (int)std::floor(gameState.visionCenterX() - vr));
  const int clipMaxX =
      std::min(WINDOW_WIDTH, (int)std::ceil(gameState.visionCenterX() + vr));
  const int clipMinY =
      std::max(0, (int)std::floor(gameState.visionCenterY() - vr));
  const int clipMaxY =
      std::min(WINDOW_HEIGHT, (int)std::ceil(gameState.visionCenterY() + vr));

  int i = 0;
  for (uint16_t y = WINDOW_HEIGHT; y > sandTop && i < (int)futures.size();
       y -= kSandTile) {
    const int rowTop = (int)y;
    const int rowBot = rowTop + kSandTile;
    if (rowBot <= clipMinY || rowTop >= clipMaxY)
      continue;

    futures[static_cast<size_t>(i)] = std::async(
        &Map::render_row, this, surface, y, clipMinX, clipMaxX, clipMinY,
        clipMaxY);
    ++i;
  }

  for (int j = 0; j < i; ++j) {
    if (futures[static_cast<size_t>(j)].valid())
      futures[static_cast<size_t>(j)].get();
  }
}

void Map::render_row(SDL_Surface *surface, uint16_t y, int clipMinX,
                     int clipMaxX, int clipMinY, int clipMaxY) {
  const int rowTop = (int)y;
  const int rowBot = rowTop + kSandTile;
  if (rowBot <= clipMinY || rowTop >= clipMaxY)
    return;

  const int align = kSandTile;
  int x = std::max(0, (clipMinX / align) * align);
  for (; x < WINDOW_WIDTH; x += align) {
    if (x + align <= clipMinX)
      continue;
    if (x >= clipMaxX)
      break;
    sandTex.draw(surface,
                 {(uint16_t)x, y, static_cast<uint16_t>(kSandTile),
                  static_cast<uint16_t>(kSandTile)},
                 {255, 255, 255, 255});
  }
}

bool Map::onBeach(float x, float y) {
  (void)x;
  const float sandTop = beachRatio * (float)WINDOW_HEIGHT;
  return y > sandTop;
}

float Map::oceanMaxY() const {
  return beachRatio * (float)WINDOW_HEIGHT;
}

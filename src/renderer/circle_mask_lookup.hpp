#pragma once

#include <SDL3/SDL.h>

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>

template <int Radius>
struct CircleMaskLookup {
  static consteval size_t pixelCount() {
    size_t count = 0;
    for (int dy = -Radius; dy <= Radius; ++dy) {
      for (int dx = -Radius; dx <= Radius; ++dx) {
        if (dx * dx + dy * dy <= Radius * Radius)
          ++count;
      }
    }
    return count;
  }

  template <size_t Count>
  static consteval std::array<SDL_Point, Count> buildOffsets() {
    std::array<SDL_Point, Count> offsets = {};
    size_t writeIndex = 0;
    for (int dy = -Radius; dy <= Radius; ++dy) {
      for (int dx = -Radius; dx <= Radius; ++dx) {
        if (dx * dx + dy * dy <= Radius * Radius)
          offsets[writeIndex++] = SDL_Point{dx, dy};
      }
    }
    return offsets;
  }

  template <size_t Count>
  static consteval std::array<int16_t, 2 * Radius + 1>
  buildMinXPerRow(const std::array<SDL_Point, Count> &offsets) {
    std::array<int16_t, 2 * Radius + 1> minX = {};
    for (int i = 0; i < 2 * Radius + 1; ++i)
      minX[i] = Radius;
    for (size_t i = 0; i < Count; ++i) {
      const int row = offsets[i].y + Radius;
      minX[row] = std::min<int16_t>(minX[row], (int16_t)offsets[i].x);
    }
    return minX;
  }

  template <size_t Count>
  static consteval std::array<int16_t, 2 * Radius + 1>
  buildMaxXPerRow(const std::array<SDL_Point, Count> &offsets) {
    std::array<int16_t, 2 * Radius + 1> maxX = {};
    for (int i = 0; i < 2 * Radius + 1; ++i)
      maxX[i] = -Radius;
    for (size_t i = 0; i < Count; ++i) {
      const int row = offsets[i].y + Radius;
      maxX[row] = std::max<int16_t>(maxX[row], (int16_t)offsets[i].x);
    }
    return maxX;
  }

  static constexpr size_t PixelCount = pixelCount();
  static constexpr std::array<SDL_Point, PixelCount> Offsets =
      buildOffsets<PixelCount>();
  static constexpr std::array<int16_t, 2 * Radius + 1> MinXPerRow =
      buildMinXPerRow<PixelCount>(Offsets);
  static constexpr std::array<int16_t, 2 * Radius + 1> MaxXPerRow =
      buildMaxXPerRow<PixelCount>(Offsets);
};

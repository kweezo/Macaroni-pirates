#pragma once

#include <cstdint>

struct PlayerMotionSample {
  uint64_t tNs = 0;
  float x = 0.0f;
  float y = 0.0f;
};

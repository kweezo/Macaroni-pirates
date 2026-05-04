#pragma once

inline bool rectsOverlap(float ax, float ay, float aw, float ah, float bx,
                         float by, float bw, float bh) {
  constexpr float slop = 0.25f;
  return ax < bx + bw + slop && ax + aw + slop > bx && ay < by + bh + slop &&
         ay + ah + slop > by;
}

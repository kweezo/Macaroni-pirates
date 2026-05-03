#include "renderer/font/font_renderer.hpp"

#include "texture/texture.hpp"

#include <SDL3/SDL.h>

#include "renderer/font/font_tex"

#include <cstring>

namespace {

constexpr int GRID_COLS = 16;
constexpr int GRID_ROWS = 8;

Texture &fontAtlasTex() {
  static Texture atlas(fontTexDat, fontTexWidth, fontTexHeight);
  return atlas;
}

int glyphIndex(unsigned char c) {
  const int cellCount = GRID_COLS * GRID_ROWS;
  const int i = (int)c;
  if (i >= cellCount)
    return (int)'?';
  return i;
}

void maskedBlit(SDL_Surface *surf, int dstW, int dstH, const Texture &srcTex,
                int sx, int sy, int sw, int sh, float dx, float dy, float dW,
                float dH, uint8_t r, uint8_t g, uint8_t b) {
  if (!surf || !surf->pixels || dW <= 0.0f || dH <= 0.0f || sw <= 0 || sh <= 0)
    return;

  const SDL_PixelFormatDetails *fmt = SDL_GetPixelFormatDetails(surf->format);
  if (!fmt)
    return;

  const int dstBpp = (int)fmt->bytes_per_pixel;
  if (dstBpp < 1 || dstBpp > 4)
    return;

  const uint8_t *src = srcTex.bytes();
  const int srcPitch = srcTex.rowPitch();
  const int dstPitch = surf->pitch;
  uint8_t *dstBase = (uint8_t *)surf->pixels;

  const int idW = (int)ceilf(dW);
  const int idH = (int)ceilf(dH);

  for (int iy = 0; iy < idH; ++iy) {
    for (int ix = 0; ix < idW; ++ix) {
      const int px = (int)floorf(dx + (float)ix);
      const int py = (int)floorf(dy + (float)iy);
      if (px < 0 || py < 0 || px >= dstW || py >= dstH)
        continue;

      const int suRaw = (int)((float)ix * (float)sw / dW);
      const int svRaw = (int)((float)iy * (float)sh / dH);
      const int su = suRaw < sw - 1 ? suRaw : sw - 1;
      const int sv = svRaw < sh - 1 ? svRaw : sh - 1;

      const uint8_t *sp =
          src + (size_t)(sy + sv) * (size_t)srcPitch + (size_t)(sx + su) * 4u;
      if (!sp[3])
        continue;

      uint8_t lum = sp[0];
      if (sp[1] > lum)
        lum = sp[1];
      if (sp[2] > lum)
        lum = sp[2];
      if (lum < 40)
        continue;

      const uint8_t dr = (uint8_t)((uint16_t)sp[0] * (uint16_t)r / 255);
      const uint8_t dg = (uint8_t)((uint16_t)sp[1] * (uint16_t)g / 255);
      const uint8_t db = (uint8_t)((uint16_t)sp[2] * (uint16_t)b / 255);

      if (dstBpp == 4) {
        const uint32_t pix = SDL_MapRGBA(fmt, nullptr, dr, dg, db, sp[3]);
        uint8_t *dp = dstBase + (size_t)py * (size_t)dstPitch + (size_t)px * 4u;
        memcpy(dp, &pix, 4u);
      } else {
        SDL_WriteSurfacePixel(surf, px, py, dr, dg, db, sp[3]);
      }
    }
  }
}

void drawFontGlyph(SDL_Surface *surf, int dstW, int dstH, const Texture &atlas,
                   int cellW, int cellH, unsigned char ch, float dx, float dy,
                   float dW, float dH, uint8_t r, uint8_t g, uint8_t b) {
  const int gi = glyphIndex(ch);
  const int col = gi % GRID_COLS;
  const int row = gi / GRID_COLS;
  const int sx = col * cellW;
  const int sy = row * cellH;
  maskedBlit(surf, dstW, dstH, atlas, sx, sy, cellW, cellH, dx, dy, dW, dH, r,
             g, b);
}
}

void FontRenderer::drawText(SDL_Surface *surface, float x, float y, float scale,
                            const char *text, uint8_t r, uint8_t g, uint8_t b,
                            float maxWidthPx) {
  if (!text || !surface || !surface->pixels)
    return;

  Texture &atlas = fontAtlasTex();

  if (scale <= 0.0f)
    scale = 1.0f;

  const int aw = atlas.widthPx();
  const int ah = atlas.heightPx();
  const int cellW = aw / GRID_COLS;
  const int cellH = ah / GRID_ROWS;
  const float outW = (float)cellW * scale;
  const float outH = (float)cellH * scale;

  const int dstW = surface->w;
  const int dstH = surface->h;

  float penX = x;
  float penY = y;
  for (const unsigned char *p = (const unsigned char *)text; *p; ++p) {
    if (*p == '\n') {
      penX = x;
      penY += outH;
      continue;
    }
    if (maxWidthPx > 0.f && penX + outW > x + maxWidthPx)
      return;
    drawFontGlyph(surface, dstW, dstH, atlas, cellW, cellH, *p, penX, penY,
                  outW, outH, r, g, b);
    penX += outW;
  }
}

float FontRenderer::estimateTextWidth(const char *text, float scale) {
  if (!text)
    return 0.0f;
  if (scale <= 0.0f)
    scale = 1.0f;
  constexpr int kCols = 16;
  const int cellW = fontTexWidth / kCols;
  return (float)cellW * scale * (float)std::strlen(text);
}

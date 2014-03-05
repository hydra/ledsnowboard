#ifndef PALETTE_H
#define PALETTE_H

#include "Rgb.h"

typedef struct Palette_s {
  uint8_t totalColors;
  RGB_t *colors;
} Palette;

#define COLORS_IN_PALETTE(colors) (sizeof(colors) / sizeof(RGB_t))

#endif

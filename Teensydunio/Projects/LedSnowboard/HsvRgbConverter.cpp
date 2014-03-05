#include <Arduino.h>


#include "Rgb.h"

#include "HsvRgbConverter.h"

RGB_t HsvRgbConverter::r;

/*
 * Source below found here: http://www.kasperkamperman.com/blog/arduino/arduino-programming-hsb-to-rgb/
 */

RGB_t &HsvRgbConverter::hsv2rgb(const HSV_t& c) {

  uint16_t val = c.v;
  uint16_t sat = 255 - c.s;
  uint32_t base;
  uint16_t hue = c.h;

  if (sat == 0) { // Acromatic color (gray). Hue doesn't mind.
    r.r = val;
    r.g = val;
    r.b = val;
  } else {

    base = ((255- sat) * val) >> 8;

    switch (hue / 60) {
    case 0:
      r.r = val;
      r.g = (((val - base) * hue) / 60) + base;
      r.b = base;
      break;
    case 1:
      r.r = (((val - base) * (60 - (hue % 60))) / 60) + base;
      r.g = val;
      r.b = base;
      break;

    case 2:
      r.r = base;
      r.g = val;
      r.b = (((val - base) * (hue % 60)) / 60) + base;
      break;

    case 3:
      r.r = base;
      r.g = (((val - base) * (60 - (hue % 60))) / 60) + base;
      r.b = val;
      break;

    case 4:
      r.r = (((val - base) * (hue % 60)) / 60) + base;
      r.g = base;
      r.b = val;
      break;

    case 5:
      r.r = val;
      r.g = base;
      r.b = (((val - base) * (60 - (hue % 60))) / 60) + base;
      break;

    }
  }
  return r;
}

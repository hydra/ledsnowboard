/*
 Copyright (c) 2012-2013 Dominic Clifton.  All right reserved.
 */

#include <Arduino.h>

#include "Config.h"

#include "Rgb.h"
#include "RgbLed.h"

void RgbLed::setColor(uint8_t r, uint8_t g, uint8_t b) {
  rgb->r = r;
  rgb->g = g;
  rgb->b = b;
}

void RgbLed::setColor(RGB_t& rgb) {
  this->rgb->r = rgb.r;
  this->rgb->g = rgb.g;
  this->rgb->b = rgb.b;
}

void RgbLed::setBrightness(uint8_t percent) {
  this->rgb->r = (uint8_t)((uint16_t)(this->rgb->r) * percent / 100);
  this->rgb->g = (uint8_t)((uint16_t)(this->rgb->g) * percent / 100);
  this->rgb->b = (uint8_t)((uint16_t)(this->rgb->b) * percent / 100);
}

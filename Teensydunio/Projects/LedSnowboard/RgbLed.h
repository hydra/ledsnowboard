/*
 Copyright (c) 2012-2013 Dominic Clifton.  All right reserved.
 */

#ifndef RgbLed_h
#define RgbLed_h

class RgbLed {
public:
  void setColor(uint8_t r, uint8_t g, uint8_t b);
  void setColor(RGB_t& rgb);
  void setBrightness(uint8_t percent);
  RGB_t *rgb;
};

#endif

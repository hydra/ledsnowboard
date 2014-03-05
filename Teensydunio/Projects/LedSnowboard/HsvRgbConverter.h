#ifndef HSVRGBCONVERTER_H_
#define HSVRGBCONVERTER_H_

#define HSV_H_MAX 360
#define HSV_S_MAX 255
#define HSV_V_MAX 255

typedef struct HSV_s {
    uint16_t h;
    uint16_t s;
    uint16_t v;
} HSV_t;

class HsvRgbConverter {
public:
  static RGB_t &hsv2rgb(const HSV_t& c);
private:
  static RGB_t r;
};

#endif

#ifndef COLORGENERATORS_H
#define COLORGENERATORS_H

#define UNDEFINED_COLOR_INDEX 0xFF

#define COLOR_ADJUST_STEP 8

class LeftRightTiltColorGenerator: public ColorGenerator {
public:
  LeftRightTiltColorGenerator(SensorDataStore& sensorDataStore);

  void reset(void);

  virtual RGB_t &generate(void);

private:
  SensorDataStore& sensorDataStore;

  uint8_t targetColorIndex;
  RGB_t targetColor;
  RGB_t currentColor;

  uint8_t chooseColorIndex(int16_t axisValue, int axisMin, int axisMax, uint8_t colorsInPalette);
  uint8_t adjustColorComponent(uint8_t current, uint8_t target);
  void adjustCurrentColor(void);
};

#endif

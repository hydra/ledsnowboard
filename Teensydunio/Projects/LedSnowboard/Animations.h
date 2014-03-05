#ifndef ANIMATIONS_H
#define ANIMATIONS_H

class SolidAnimation: public Animation {
public:
  typedef Animation super;

  SolidAnimation(ColorGenerator& colorGenerator);
  virtual void frameBegin(void);
  virtual void updateLedColor(uint8_t x, uint8_t y, RgbLed& rgbLed);

private:
  RGB_t* color;
  ColorGenerator& colorGenerator;
};

class FlashAnimation: public Animation {
public:
  typedef Animation super;

  FlashAnimation(ColorGenerator& colorGenerator);
  virtual void frameBegin(void);
  virtual void updateLedColor(uint8_t x, uint8_t y, RgbLed& rgbLed);

private:
  RGB_t* color;
  ColorGenerator& colorGenerator;
};

class SawtoothFadeAnimation: public Animation {
public:
  typedef Animation super;

  SawtoothFadeAnimation(ColorGenerator& colorGenerator);

  virtual void frameBegin(void);

  virtual void updateLedColor(uint8_t x, uint8_t y, RgbLed& rgbLed);

private:
  RGB_t* color;
  ColorGenerator& colorGenerator;
  uint8_t brightness;
};

class UpDownFadeAnimation: public Animation {
public:
  typedef Animation super;

  UpDownFadeAnimation(ColorGenerator& colorGenerator);
  virtual void frameBegin(void);
  virtual void updateLedColor(uint8_t x, uint8_t y, RgbLed& rgbLed);

private:
  RGB_t* color;
  ColorGenerator& colorGenerator;
  uint8_t brightness;
};

class LineTopToBottomAnimation: public Animation {
public:
  typedef Animation super;

  LineTopToBottomAnimation(ColorGenerator& colorGenerator, uint8_t gridHeight);
  virtual void frameBegin(void);
  virtual void updateLedColor(uint8_t x, uint8_t y, RgbLed& rgbLed);

private:
  RGB_t* color;
  ColorGenerator& colorGenerator;
};

class ThickLineTopToBottomAnimation: public Animation {
public:
  typedef Animation super;

  ThickLineTopToBottomAnimation(ColorGenerator& colorGenerator, uint8_t gridHeight);
  virtual void frameBegin(void);
  virtual void updateLedColor(uint8_t x, uint8_t y, RgbLed& rgbLed);

private:
  RGB_t* color;
  ColorGenerator& colorGenerator;
};

class CarLightsAnimation : public Animation {

public:
  CarLightsAnimation(SensorDataStore& sensorDataStore, uint8_t gridWidth, uint8_t gridHeight);
  virtual void frameBegin(void);
  virtual void updateLedColor(uint8_t x, uint8_t y, RgbLed& rgbLed);

private:
  SensorDataStore& sensorDataStore;
  uint8_t gridHeight;
  uint8_t gridWidth;

  uint8_t state;
  bool forwards;
};

class MovingRainbowAnimation : public Animation {

public:
  MovingRainbowAnimation(SensorDataStore& sensorDataStore, uint8_t gridHeight);
  virtual void frameBegin(void);
  virtual void updateLedColor(uint8_t x, uint8_t y, RgbLed& rgbLed);
  virtual unsigned long getFrameDelayMicros(void);

private:
  SensorDataStore& sensorDataStore;
  uint8_t gridHeight;

  uint8_t xOffset;
  uint8_t yOffset;

  HSV_t hsv;
  uint8_t framePercent;

  unsigned long additionalFrameDelay;
};


#endif


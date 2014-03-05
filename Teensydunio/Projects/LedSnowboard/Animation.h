
#ifndef ANIMATION_H
#define ANIMATION_H

#define DEFAULT_FRAME_DELAY 2500
#define DEFAULT_FRAME_DELAY_STEP 500

#define MIN_FRAME_DELAY 0L
#define MAX_FRAME_DELAY (1000L * 1000L)

class Animation {

public:
  Animation();

  virtual void reset(void);

  virtual void frameBegin(void);
  virtual void updateLedColor(uint8_t x, uint8_t y, RgbLed& rgbLed);
  virtual void frameEnd(void);

  virtual void decreaseFrameDelay(void);
  virtual void increaseFrameDelay(void);
  virtual unsigned long getFrameDelayMicros(void);

protected:
  uint16_t frameCounter;
  uint16_t totalFrames;
  unsigned long frameDelay;
  unsigned int frameDelayStep;

  void clampFrameDelay(void);
  void setTotalFrames(uint16_t totalFrames);
};

#endif

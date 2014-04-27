#include <Arduino.h>

#include "SensorDataStore.h"

#include "Rgb.h"
#include "RgbLed.h"
#include "HsvRgbConverter.h"
#include "ColorGenerator.h"

#include "ScaleMath.h"

#include "Animation.h"

#include "Animations.h"


#define DEFAULT_FLASH_ANIM_DELAY 25000
#define DEFAULT_FLASH_ANIM_DELAY_STEP 2000

#define DEFAULT_CAR_ANIM_DELAY 25000
#define DEFAULT_CAR_ANIM_DELAY_STEP 2000

#define DEFAULT_RAINBOW_ANIM_DELAY 0

#define FADE_ANIM_BRIGHTNESS_LEVELS 50

SolidAnimation::SolidAnimation(ColorGenerator& colorGenerator) :
    colorGenerator(colorGenerator) {
  setTotalFrames(1);
}

void SolidAnimation::frameBegin(void) {
  super::frameBegin();
  color = &colorGenerator.generate();
}

void SolidAnimation::updateLedColor(uint8_t x, uint8_t y, RgbLed& rgbLed) {
  rgbLed.setColor(*color);
}

FlashAnimation::FlashAnimation(ColorGenerator& colorGenerator) :
    colorGenerator(colorGenerator) {
  setTotalFrames(2);
  frameDelay = DEFAULT_FLASH_ANIM_DELAY;
  frameDelayStep = DEFAULT_FLASH_ANIM_DELAY_STEP;
}

void FlashAnimation::frameBegin(void) {
  super::frameBegin();
  color = &colorGenerator.generate();
}

void FlashAnimation::updateLedColor(uint8_t x, uint8_t y, RgbLed& rgbLed) {
  if (frameCounter == 1) {
    rgbLed.setColor(*color);
    return;
  }
  rgbLed.setColor(0, 0, 0);
}

SawtoothFadeAnimation::SawtoothFadeAnimation(ColorGenerator& colorGenerator) :
    colorGenerator(colorGenerator) {
  setTotalFrames(FADE_ANIM_BRIGHTNESS_LEVELS);
}

void SawtoothFadeAnimation::frameBegin(void) {
  super::frameBegin();
  color = &colorGenerator.generate();
  brightness = (uint8_t) ((int) (frameCounter) * 100 / (int) FADE_ANIM_BRIGHTNESS_LEVELS);

}

void SawtoothFadeAnimation::updateLedColor(uint8_t x, uint8_t y, RgbLed& rgbLed) {
  rgbLed.setColor(*color);
  rgbLed.setBrightness(brightness);
}

UpDownFadeAnimation::UpDownFadeAnimation(ColorGenerator& colorGenerator) :
    colorGenerator(colorGenerator) {
  setTotalFrames(FADE_ANIM_BRIGHTNESS_LEVELS * 2);
}

void UpDownFadeAnimation::frameBegin(void) {
  super::frameBegin();
  color = &colorGenerator.generate();

  int halfTotalFrames = (totalFrames / 2);
  int z = halfTotalFrames - frameCounter;
  int t = halfTotalFrames - abs(z);

  brightness = (uint8_t) ((int) (t) * 100 / (int) FADE_ANIM_BRIGHTNESS_LEVELS);

}

void UpDownFadeAnimation::updateLedColor(uint8_t x, uint8_t y, RgbLed& rgbLed) {
  rgbLed.setColor(*color);
  rgbLed.setBrightness(brightness);
}

LineTopToBottomAnimation::LineTopToBottomAnimation(ColorGenerator& colorGenerator, uint8_t gridHeight) :
    colorGenerator(colorGenerator) {
  setTotalFrames(gridHeight);
}

void LineTopToBottomAnimation::frameBegin(void) {
  super::frameBegin();
  color = &colorGenerator.generate();
}

void LineTopToBottomAnimation::updateLedColor(uint8_t x, uint8_t y, RgbLed& rgbLed) {
  if (frameCounter == y) {
    rgbLed.setColor(*color);
    return;
  }

  rgbLed.setColor(0, 0, 0);
}

ThickLineTopToBottomAnimation::ThickLineTopToBottomAnimation(ColorGenerator& colorGenerator, uint8_t gridHeight) :
    colorGenerator(colorGenerator) {
  setTotalFrames(gridHeight + 9);
}

void ThickLineTopToBottomAnimation::frameBegin(void) {
  super::frameBegin();
  color = &colorGenerator.generate();
}

void ThickLineTopToBottomAnimation::updateLedColor(uint8_t x, uint8_t y, RgbLed& rgbLed) {
  rgbLed.setColor(*color);

  if (frameCounter == y + 5) {
    rgbLed.setBrightness(100);
  } else if (frameCounter == y + 4 || frameCounter == y + 6) {
    rgbLed.setBrightness(75);
  } else if (frameCounter == y + 3 || frameCounter == y + 7) {
    rgbLed.setBrightness(50);
  } else if (frameCounter == y + 2 || frameCounter == y + 8) {
    rgbLed.setBrightness(25);
  } else if (frameCounter == y + 1 || frameCounter == y + 9) {
    rgbLed.setBrightness(12);
  } else {
    rgbLed.setBrightness(0);
  }
}

enum CarDirectionStates {
  TURNING_LEFT = 0, STRAIGHT_AHEAD, TURNING_RIGHT
};

#define INDICATOR_WIDTH 3
#define INDICATOR_HEIGHT 13

#define FRAMES_PER_BLINK_ON_AND_OFF 10
#define FRAMES_PER_BLINK_ON 5

#define RIGHT_TILT_THRESHOLD -3000
#define LEFT_TILT_THRESHOLD 3000

#define FORWARD_TILT_THRESHOLD 6000
#define BACKWARD_TILT_THRESHOLD -6000

CarLightsAnimation::CarLightsAnimation(SensorDataStore& sensorDataStore, uint8_t gridWidth, uint8_t gridHeight) :
    sensorDataStore(sensorDataStore),gridHeight(gridHeight),  gridWidth(gridWidth), forwards(true) {
  setTotalFrames(FRAMES_PER_BLINK_ON_AND_OFF);
  frameDelay = DEFAULT_CAR_ANIM_DELAY;
  frameDelayStep = DEFAULT_CAR_ANIM_DELAY_STEP;
}

void CarLightsAnimation::frameBegin(void) {

  int16_t x = sensorDataStore.sampledAccelerationData.x;
  int16_t y = sensorDataStore.sampledAccelerationData.y;

  if (y > FORWARD_TILT_THRESHOLD) {
    forwards = true;
  } else if (y < BACKWARD_TILT_THRESHOLD) {
    forwards = false;
  }

  if (x < RIGHT_TILT_THRESHOLD) {
    state = TURNING_RIGHT;
  } else if (x > LEFT_TILT_THRESHOLD) {
    state = TURNING_LEFT;
  } else {
    state = STRAIGHT_AHEAD;
  }

}

void CarLightsAnimation::updateLedColor(uint8_t x, uint8_t y, RgbLed& rgbLed) {
  RGB_t white = {255, 255, 255};
  RGB_t red = {255, 0, 0};
  RGB_t amber = {255, 147, 0};
  RGB_t black = {0, 0, 0};

  if (y < INDICATOR_HEIGHT || y >= gridHeight - INDICATOR_HEIGHT) {

    if (y < INDICATOR_HEIGHT) {
      if (forwards) {
        rgbLed.setColor(white);
      } else {
        rgbLed.setColor(red);
      }
    } else if (y >= gridHeight - INDICATOR_HEIGHT) {
      if (forwards) {
        rgbLed.setColor(red);
      } else {
        rgbLed.setColor(white);
      }

    }

    // LEFT / RIGHT
    if ((x < INDICATOR_WIDTH && state == TURNING_LEFT) || (x > (gridWidth - 1) - INDICATOR_WIDTH && state == TURNING_RIGHT)) {
      if (frameCounter % FRAMES_PER_BLINK_ON_AND_OFF < FRAMES_PER_BLINK_ON) {
        rgbLed.setColor(amber);
      } else {
        rgbLed.setColor(black);
      }
    }
    return;
  }

  uint8_t rowsBetweenIndicators = gridHeight - (INDICATOR_HEIGHT * 2);
  uint8_t z = y - INDICATOR_HEIGHT;

  int p = (rowsBetweenIndicators / 2) - z;

  if ((forwards && p >= 0) || (!forwards && p < 0)) {
    rgbLed.setColor(white);
  } else {
    rgbLed.setColor(red);
  }

  p = abs(p);
  uint8_t brightness = (p * 100) / rowsBetweenIndicators;

  rgbLed.setBrightness(brightness);
}

#define AXIS_X_MIN -4000
#define AXIS_X_MAX 4000

#define MAX_RAINBOW_FRAME_DELAY_MS 50

MovingRainbowAnimation::MovingRainbowAnimation(SensorDataStore& sensorDataStore, uint8_t gridHeight) : sensorDataStore(sensorDataStore), gridHeight(gridHeight), additionalFrameDelay(0) {
  setTotalFrames(gridHeight);
  frameDelay = DEFAULT_RAINBOW_ANIM_DELAY;
  hsv.s = 0;
  hsv.v = HSV_V_MAX;
}

unsigned long MovingRainbowAnimation::getFrameDelayMicros(void) {
  return frameDelay + additionalFrameDelay;
}

void MovingRainbowAnimation::frameBegin(void) {
  xOffset = 0;
  yOffset = 0;

  int16_t limitedAxisValue = sensorDataStore.sampledAccelerationData.x;
  if (limitedAxisValue < AXIS_X_MIN) {
    limitedAxisValue = AXIS_X_MIN;
  } else if (limitedAxisValue > AXIS_X_MAX) {
    limitedAxisValue = AXIS_X_MAX;
  }
  int temp = scaleRange(limitedAxisValue, AXIS_X_MIN, AXIS_X_MAX, 0 - MAX_RAINBOW_FRAME_DELAY_MS, MAX_RAINBOW_FRAME_DELAY_MS);

  additionalFrameDelay = (MAX_RAINBOW_FRAME_DELAY_MS - abs(temp)) * 1000L;

  framePercent = ((uint32_t)frameCounter * 100) / totalFrames;
}

void MovingRainbowAnimation::updateLedColor(uint8_t x, uint8_t y, RgbLed& rgbLed) {

  uint8_t yPercent = ((uint32_t)y * 100) / gridHeight;

  int16_t percent = framePercent + yPercent;
  if (percent > 100) {
    percent = percent - 100;
  }

  hsv.h = ((uint32_t)percent * HSV_H_MAX) / 100;

  RGB_t& rgb = HsvRgbConverter::hsv2rgb(hsv);

  rgbLed.setColor(rgb);
}

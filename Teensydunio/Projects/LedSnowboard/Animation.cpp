#include <Arduino.h>

#include "Rgb.h"
#include "RgbLed.h"

#include "Animation.h"

Animation::Animation() :
    frameCounter(0), totalFrames(0), frameDelay(DEFAULT_FRAME_DELAY), frameDelayStep(DEFAULT_FRAME_DELAY_STEP) {
}

void Animation::reset(void) {
  frameCounter = 0;
}

void Animation::frameBegin(void) {
  //Serial.print("frameCounter: ");
  //Serial.println(frameCounter);
}

void Animation::updateLedColor(uint8_t x, uint8_t y, RgbLed& rgbLed) {
}

void Animation::frameEnd(void) {
  frameCounter++;
  if (frameCounter > totalFrames) {
    frameCounter = 0;
  }
}

void Animation::setTotalFrames(uint16_t totalFrames) {
  this->totalFrames = totalFrames;
}

void Animation::decreaseFrameDelay(void) {
  if (frameDelay < frameDelayStep) {
    return;
  }
  frameDelay -= frameDelayStep;
  clampFrameDelay();
}

void Animation::increaseFrameDelay(void) {
  if (frameDelay > MAX_FRAME_DELAY - frameDelayStep) {
    return;
  }
  frameDelay += frameDelayStep;
  clampFrameDelay();
}

unsigned long Animation::getFrameDelayMicros(void) {
  return frameDelay;
}

void Animation::clampFrameDelay(void) {
  if (frameDelay < MIN_FRAME_DELAY) {
    frameDelay = MIN_FRAME_DELAY;
  }
  if (frameDelay > MAX_FRAME_DELAY) {
    frameDelay = MAX_FRAME_DELAY;
  }
}


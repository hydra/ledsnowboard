/*
 Copyright (c) 2012-2013 Dominic Clifton.  All right reserved.
 */

#ifndef DebouncedInput_h
#define DebouncedInput_h

#include "Input.h"

class DebouncedInput {
public:

  DebouncedInput() : input(NULL), previousTime(0), debounceDelayMillis(50), previousValue(false), value(false) {
  }

  void setInput(Input *input) {
    this->input = input;
  }

  void setDebounceDelay(int debounceDelay) {
    this->debounceDelayMillis = debounceDelay;
  }

  boolean getValue(void) {
    input->read();
    boolean rawValue = input->getBoolValue();

    if (rawValue != previousValue) {
      previousTime = millis();
    }

    if ((millis() - previousTime) >= debounceDelayMillis) {
      value = rawValue;
    }

    previousValue = rawValue;

    return value;
  }

protected:
  Input *input;

  uint32_t previousTime;
  uint32_t debounceDelayMillis;
  bool previousValue;
  bool value;
};

#endif

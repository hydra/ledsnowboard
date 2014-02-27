/*
 Copyright (c) 2012-2013 Dominic Clifton.  All right reserved.
 */

#ifndef DebouncedInput_h
#define DebouncedInput_h

#include "Input.h"

class DebouncedInput {
public:

    DebouncedInput() : input(NULL), valueIsValidAt(0), debounceDelayMillis(50), previousValue(false), value(false) {
    }

    void setInput(Input *input) {
        this->input = input;
        reset();
    }

    void setDebounceDelay(uint16_t debounceDelayMillis) {
        this->debounceDelayMillis = debounceDelayMillis;
        reset();
    }

    void reset() {
        valueIsValidAt = millis() + debounceDelayMillis;

        input->read();
        value = input->getBoolValue();
        previousValue = value;
    }

    boolean getValue(void) {

        input->read();
        boolean rawValue = input->getBoolValue();

        uint32_t now = millis();

        int32_t signedDiff = now - valueIsValidAt;

        if (signedDiff < 0L) {
            return value;
        }

        if (rawValue != previousValue) {
            value = rawValue;
        }

        valueIsValidAt = now + debounceDelayMillis;

        previousValue = rawValue;

        return value;
    }

    uint16_t getDebounceDelayMillis(void) {
        return debounceDelayMillis;
    }

protected:
    Input *input;

    uint32_t valueIsValidAt;
    uint16_t debounceDelayMillis;
    bool previousValue;
    bool value;
};

#endif

/*
 * animation.c
 *
 *  Created on: 14 Jan 2014
 *      Author: hydra
 */

#include "WProgram.h"

#include <OctoWS2811.h>
#include "AccelGyro.h"
#include "Time.h"

#include "Animator.h"

#include "Animations.h"

extern OctoWS2811 leds;
extern AccelGyro accelGyro;

unsigned char readByteUnsignedChar(int* aPosition) {
    unsigned char readByte = (*(const unsigned char *)(animationData + (*(aPosition))++));
    if (readByte == ESCAPE_BYTE) {
        readByte = (*(const unsigned char *)(animationData + (*(aPosition))++));
        readByte = readByte ^ XOR_BYTE;
    }

    return readByte;
}

signed char readByteSignedChar(int* aPosition) {
    signed char readByte = (*(const unsigned char *)(animationData + (*(aPosition))++));
    if (readByte == ESCAPE_BYTE) {
        readByte = (*(const unsigned char *)(animationData + (*(aPosition))++));
        readByte = readByte ^ XOR_BYTE;
    }

    return readByte;
}



void Animator::initializeFunctionData(unsigned int functionCount, unsigned int colorComponentCount) {
    for (unsigned int i = 0; i < functionCount; i++) {
        for (unsigned int j = 0; j < colorComponentCount; j++) {
            iFunctions[i][j] = 0;
        }
    }
}

void Animator::initializeValueAxisData(unsigned int ledsInAnimation, unsigned int valuesInRange) {
    for (unsigned int i = 0; i < ledsInAnimation; i++) {
        for (unsigned int j = 0; j < valuesInRange; j++) {
            iValueAxisData[i][j] = 0;
        }
    }
}


void Animator::readAnimationDetails() {
    timeAxisNum = -1;

    initializeValueAxisData(COUNT_OF_LEDS_IN_ANIMATION, MAX_VALUES_IN_RANGE_USED_BY_ANIMATION);
    initializeFunctionData(COUNT_OF_FUNCTIONS_IN_ANIMATION, COLOR_COMPONENT_COUNT);

    if (readByteUnsignedChar(&animationByteOffset) != HEADER_BYTE) {
        //throw new InvalidAnimationException("No header byte");
    }

    unsigned char numLedsHigh = readByteUnsignedChar(&animationByteOffset);
    unsigned char numLedsLow = readByteUnsignedChar(&animationByteOffset);

    ledCount = numLedsHigh |= numLedsLow << 8;
    Serial.print("led count is ");
    Serial.print(ledCount, DEC);
    Serial.print("\n");

    iNumFunctions = readByteUnsignedChar(&animationByteOffset);
    Serial.print("function count is ");
    Serial.print(iNumFunctions, DEC);
    Serial.print("\n");
    for (int i = 0; i < iNumFunctions; i++) {
        readFunctionData(i);
    }

    valueAxisCount = readByteUnsignedChar(&animationByteOffset);
    Serial.print("value axis count is ");
    Serial.print(valueAxisCount, DEC);
    Serial.print("\n");

    readTimeAxis();
    
    for (signed int valueAxisIndex = 0; valueAxisIndex < valueAxisCount;
            valueAxisIndex++) {
        readValueAxis(valueAxisIndex);
    }
    
    animationByteOffsetOfFirstFrame = animationByteOffset;

    frameIndex = timeAxisLowValue;
}

void Animator::readFunctionData(int num) {
    unsigned char red1 = readByteUnsignedChar(&animationByteOffset);
    unsigned char red2 = readByteUnsignedChar(&animationByteOffset);
    unsigned char red3 = readByteUnsignedChar(&animationByteOffset);
    unsigned char red4 = readByteUnsignedChar(&animationByteOffset);

    unsigned int r = red1;
    r |= red2 << 8;
    r |= red3 << 16;
    r |= red4 << 24;

    signed int redIncrement = r;

    unsigned char green1 = readByteUnsignedChar(&animationByteOffset);
    unsigned char green2 = readByteUnsignedChar(&animationByteOffset);
    unsigned char green3 = readByteUnsignedChar(&animationByteOffset);
    unsigned char green4 = readByteUnsignedChar(&animationByteOffset);

    unsigned int g = green1;
    g |= green2 << 8;
    g |= green3 << 16;
    g |= green4 << 24;
    signed int greenIncrement = g;

    //green1 |= green2 << 8;
    //greenIncrement = greenIncrement |= green3 << 16;
    //greenIncrement = greenIncrement |= green4 << 24;

    unsigned char blue1 = readByteUnsignedChar(&animationByteOffset);
    unsigned char blue2 = readByteUnsignedChar(&animationByteOffset);
    unsigned char blue3 = readByteUnsignedChar(&animationByteOffset);
    unsigned char blue4 = readByteUnsignedChar(&animationByteOffset);

    unsigned int b = blue1;
    b |= blue2 << 8;
    b |= blue3 << 16;
    b |= blue4 << 24;

    signed int blueIncrement = b;

    Serial.print(redIncrement, DEC);
    Serial.print(" ");
    Serial.print(greenIncrement, DEC);
    Serial.print(" ");
    Serial.print(blueIncrement, DEC);
    Serial.print("\n");

    iFunctions[num][0] = redIncrement;
    iFunctions[num][1] = greenIncrement;
    iFunctions[num][2] = blueIncrement;
}

void Animator::readAndSetColour(int ledNum) {
    
    unsigned char red = readByteUnsignedChar(&animationByteOffset);
    unsigned char green = readByteUnsignedChar(&animationByteOffset);
    unsigned char blue = readByteUnsignedChar(&animationByteOffset);

#ifdef DEBUG_ANIMATOR_CODEC
    Serial.print(ledNum, DEC);
    Serial.print(" : ");
    Serial.print(red, HEX);
    Serial.print(" ");
    Serial.print(green, HEX);
    Serial.print(" ");
    Serial.print(blue, HEX);
    Serial.print("\n");
#endif
    
    if (iBackgroundColour
            && (red != iBackgroundColourRed || green != iBackgroundColourGreen
                    || blue != iBackgroundColourBlue)) {

        signed char accelerometerValue = accelGyro.getNormalisedAccelerometerXValue();

        signed int redIncrement = 0;
        signed int greenIncrement = 0;
        signed int blueIncrement = 0;

        int initialValue = 0;
        int highValue = 0;
        if (accelerometerValue < 0) {
            initialValue = accelerometerValue;
            highValue = valueAxisZeroValue;
        } else if (accelerometerValue > 0) {
            initialValue = valueAxisZeroValue + 1;
            highValue = accelerometerValue + 1;
        }

        for (int frame = initialValue; frame < highValue; frame++) {
            int functionNum = iValueAxisData[ledNum][frame + valueAxisOffset];
            //Serial.print(functionNum, DEC);
            //Serial.print(":");
            redIncrement += iFunctions[functionNum][0];
            greenIncrement += iFunctions[functionNum][1];
            blueIncrement += iFunctions[functionNum][2];
        }

#ifdef DEBUG_ANIMATOR_CODEC
        Serial.print(redIncrement, DEC);
        Serial.print(" ");
        Serial.print(greenIncrement, DEC);
        Serial.print(" ");
        Serial.print(blueIncrement, DEC);
        Serial.print("\n");*/
#endif
        
        if (redIncrement < -65536) {
            redIncrement = 0;
        }

        if (redIncrement > 65536) {
            redIncrement = 65536;
        }

        if (greenIncrement < -65536) {
            greenIncrement = 0;
        }

        if (greenIncrement > 65536) {
            greenIncrement = 65536;
        }

        if (blueIncrement < -65536) {
            blueIncrement = 0;
        }

        if (blueIncrement > 65536) {
            blueIncrement = 65536;
        }

        int redBig = red * 256;
        int greenBig = green * 256;
        int blueBig = blue * 256;

        redBig += redIncrement;
        if (redBig < 0) {
            redBig = 0;
        }

        if (redBig > 65536) {
            redBig = 65536;
        }

        greenBig += greenIncrement;
        if (greenBig < 0) {
            greenBig = 0;
        }

        if (greenBig > 65536) {
            greenBig = 65536;
        }

        blueBig += blueIncrement;
        if (greenBig < 0) {
            greenBig = 0;
        }

        if (greenBig > 65536) {
            greenBig = 65536;
        }

        redBig = redBig / 256;
        greenBig = greenBig / 256;
        blueBig = blueBig / 256;

        red = redBig;
        green = greenBig;
        blue = blueBig;
    }

    leds.setPixel(ledNum, red, green, blue);
}

void Animator::beginReadAxis(void) {
    int axisType = readByteUnsignedChar(&animationByteOffset);
    int priority = readByteUnsignedChar(&animationByteOffset);
    boolean opaque = (boolean) readByteUnsignedChar(&animationByteOffset);
}

void Animator::readTimeAxis(void) {
    beginReadAxis();

    timeAxisLowValue = readByteSignedChar(&animationByteOffset);
    Serial.print("time axis low value : ");
    Serial.print(timeAxisLowValue, DEC);
    Serial.print("\n");
    timeAxisHighValue = readByteSignedChar(&animationByteOffset);
    Serial.print("time axis high value : ");
    Serial.print(timeAxisHighValue, DEC);
    Serial.print("\n");
    iTimeAxisSpeed = readByteSignedChar(&animationByteOffset);
    Serial.print("time axis speed : ");
    Serial.print(iTimeAxisSpeed, DEC);
    Serial.print("\n");

    iBackgroundColour = readByteUnsignedChar(&animationByteOffset);
    if (iBackgroundColour) {
        Serial.print("background colour : ");
        iBackgroundColourRed = readByteUnsignedChar(&animationByteOffset);
        iBackgroundColourGreen = readByteUnsignedChar(&animationByteOffset);
        iBackgroundColourBlue = readByteUnsignedChar(&animationByteOffset);

        Serial.print(iBackgroundColourRed, HEX);
        Serial.print(" ");
        Serial.print(iBackgroundColourGreen, HEX);
        Serial.print(" ");
        Serial.print(iBackgroundColourBlue, HEX);
        Serial.print("\n");
    }
}

void Animator::readValueAxis(unsigned int valueAxisIndex) {
    beginReadAxis();

    valueAxisLowValue = readByteSignedChar(&animationByteOffset);
    Serial.print("value axis low value : ");
    Serial.print(valueAxisLowValue, DEC);
    Serial.print("\n");
    valueAxisHighValue = readByteSignedChar(&animationByteOffset);
    Serial.print("value axis high value : ");
    Serial.print(valueAxisHighValue, DEC);
    Serial.print("\n");
    valueAxisZeroValue = readByteSignedChar(&animationByteOffset);
    Serial.print("value axis zero value : ");
    Serial.print(valueAxisZeroValue, DEC);
    Serial.print("\n");

    valueAxisOffset = -valueAxisLowValue;
    Serial.print("valueAxisOffset: ");
    Serial.print(valueAxisOffset, DEC);
    Serial.print("\n");

    for (int frame = valueAxisLowValue; frame <= valueAxisHighValue;
            frame++) {
        Serial.print(frame, DEC);
        Serial.print("\n");
        int ledNum = INITIAL_LED;
        for (int ledIndex = 0; ledIndex < ledCount; ledIndex++) {
            char ledNumber = readByteUnsignedChar(&animationByteOffset);
            Serial.print("ledNumber:");
            Serial.print(ledNumber, DEC);
            Serial.print("\n");

            unsigned char frameType = readByteUnsignedChar(
                    &animationByteOffset);

            int valueAxisIndex = valueAxisOffset + frame;
            Serial.print("valueAxisIndex:");
            Serial.print(valueAxisIndex, DEC);
            Serial.print("\n");

            switch (frameType) {
            case FT_FUNCTION:
                iValueAxisData[ledIndex][valueAxisIndex] =
                        readByteUnsignedChar(&animationByteOffset);
                if(iValueAxisData[ledIndex][valueAxisIndex] != 0) {
                    Serial.print("function is ");
                    Serial.print(iValueAxisData[ledIndex][valueAxisIndex], DEC);
                    Serial.print("\n");
                }
                break;
            case FT_LINKED:
                iValueAxisData[ledIndex][valueAxisIndex] = 255;
                break;
            }
        }
    }
}

void Animator::renderNextFrame() {
    // Serial.print("Processing frame: ");
    // Serial.print(frameIndex, DEC);
    // Serial.print("\n");
    processFrame(frameIndex);

    frameIndex++;
    
    if (frameIndex > timeAxisHighValue) {
        // if (readByteUnsignedChar(&iCounter) != TERMINATING_BYTE) {
        //throw new InvalidAnimationException("No terminating byte");
        // }
        
        // rewind after last frame
        frameIndex = timeAxisLowValue;
        animationByteOffset = animationByteOffsetOfFirstFrame;
    }
}

void Animator::processFrame(unsigned int frameIndex) {
    //int ledNum = kInitialLed;
    for (int i = 0; i < ledCount; i++) {
        int ledNum = readByteUnsignedChar(&animationByteOffset); // led number
        //Serial.print(ledNum, DEC);
        //Serial.print("\n");

        unsigned char frameType = readByteUnsignedChar(
                &animationByteOffset);

        switch (frameType) {
            case FT_FUNCTION:
                //readFunctionAndSetColour();
                break;
            case FT_COLOUR:
                readAndSetColour(i);
                break;
        }
    }
}


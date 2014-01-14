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

#include "Animation.h"


extern OctoWS2811 leds;
extern AccelGyro accelGyro;

PROGMEM prog_uchar animation1[] = { 86, 1, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 1, 0, 255, 255, 255, 255, 0, 0, 0, 0, 0, 0, 1, 0, 255, 255, 0, 0,
        0, 0, 255, 255, 0, 0, 1, 0, 156, 0, 0, 9, 100, 1, 0, 0, 0, 1, 156, 0,
        255, 1, 0, 1, 2, 34, 1, 1, 3, 1, 2, 34, 2, 34, 1, 1, 255, 0, 0, 1, 1, 0,
        0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 255, 0, 0, 1, 1, 255, 0, 0, 1,
        1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 255, 0, 0, 69 };

unsigned char iValueAxisData[16][201];

signed int iFunctions[10][3];

int animationByteOffset;
int timeAxisNum;
int iNumValueAxes;
int numLeds;

int iTopOfTimeAxis;

int iNumFunctions;

signed char iValueAxisLowValue;
signed char iValueAxisHighValue;
signed char iValueAxisZeroValue;

int iValueAxisOffset;

int iTimeAxisLowValue;
int iTimeAxisHighValue;
int iTimeAxisSpeed;

bool iBackgroundColour;
unsigned char iBackgroundColourRed;
unsigned char iBackgroundColourGreen;
unsigned char iBackgroundColourBlue;

void readAnimationDetails() {
    timeAxisNum = -1;

    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 201; j++) {
            iValueAxisData[i][j] = 0;
        }
    }

    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 3; j++) {
            iFunctions[i][j] = 0;
        }
    }

    if (readByteUnsignedChar(&animationByteOffset) != HEADER_BYTE) {
        //throw new InvalidAnimationException("No header byte");
    }

    unsigned char numLedsHigh = readByteUnsignedChar(&animationByteOffset);
    unsigned char numLedsLow = readByteUnsignedChar(&animationByteOffset);

    numLeds = numLedsHigh |= numLedsLow << 8;
    Serial.print("led count is ");
    Serial.print(numLeds, DEC);
    Serial.print("\n");

    iNumFunctions = readByteUnsignedChar(&animationByteOffset);
    Serial.print("function count is ");
    Serial.print(iNumFunctions, DEC);
    Serial.print("\n");
    for (int i = 0; i < iNumFunctions; i++) {
        readFunctionData(i);
    }

    iNumValueAxes = readByteUnsignedChar(&animationByteOffset);
    Serial.print("value axis count is ");
    Serial.print(iNumValueAxes, DEC);
    Serial.print("\n");

    readTimeAxis();

    for (signed int valueAxisIndex = 0; valueAxisIndex < iNumValueAxes;
            valueAxisIndex++) {
        readValueAxis(valueAxisIndex);
    }
}

void readFunctionData(int num) {
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

void readAndSetColour(int ledNum) {
    Serial.print(ledNum, DEC);
    Serial.print(" : ");

    unsigned char red = readByteUnsignedChar(&animationByteOffset);
    unsigned char green = readByteUnsignedChar(&animationByteOffset);
    unsigned char blue = readByteUnsignedChar(&animationByteOffset);

    Serial.print(red, HEX);
    Serial.print(" ");
    Serial.print(green, HEX);
    Serial.print(" ");
    Serial.print(blue, HEX);
    Serial.print("\n");

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
            highValue = iValueAxisZeroValue;
        } else if (accelerometerValue > 0) {
            initialValue = iValueAxisZeroValue + 1;
            highValue = accelerometerValue + 1;
        }

        for (int frame = initialValue; frame < highValue; frame++) {
            int functionNum = iValueAxisData[ledNum][frame + iValueAxisOffset];
            //Serial.print(functionNum, DEC);
            //Serial.print(":");
            redIncrement += iFunctions[functionNum][0];
            greenIncrement += iFunctions[functionNum][1];
            blueIncrement += iFunctions[functionNum][2];
        }

        /*Serial.print(redIncrement, DEC);
         Serial.print(" ");
         Serial.print(greenIncrement, DEC);
         Serial.print(" ");
         Serial.print(blueIncrement, DEC);
         Serial.print("\n");*/

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

void beginReadAxis(void) {
    int axisType = readByteUnsignedChar(&animationByteOffset);
    int priority = readByteUnsignedChar(&animationByteOffset);
    boolean opaque = (boolean) readByteUnsignedChar(&animationByteOffset);
}

void readTimeAxis(void) {
    beginReadAxis();

    iTimeAxisLowValue = readByteSignedChar(&animationByteOffset);
    Serial.print("time axis low value : ");
    Serial.print(iTimeAxisLowValue, DEC);
    Serial.print("\n");
    iTimeAxisHighValue = readByteSignedChar(&animationByteOffset);
    Serial.print("time axis high value : ");
    Serial.print(iTimeAxisHighValue, DEC);
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

void readValueAxis(signed int valueAxisIndex) {
    beginReadAxis();

    iValueAxisLowValue = readByteSignedChar(&animationByteOffset);
    Serial.print("value axis low value : ");
    Serial.print(iValueAxisLowValue, DEC);
    Serial.print("\n");
    iValueAxisHighValue = readByteSignedChar(&animationByteOffset);
    Serial.print("value axis high value : ");
    Serial.print(iValueAxisHighValue, DEC);
    Serial.print("\n");
    iValueAxisZeroValue = readByteSignedChar(&animationByteOffset);
    Serial.print("value axis zero value : ");
    Serial.print(iValueAxisZeroValue, DEC);
    Serial.print("\n");

    iValueAxisOffset = -iValueAxisLowValue;

    for (int frame = iValueAxisLowValue; frame <= iValueAxisHighValue;
            frame++) {
        //  Serial.print(frame, DEC);
        //   Serial.print("\n");
        int ledNum = INITIAL_LED;
        for (int i = 0; i < numLeds; i++) {
            char ledNum = readByteUnsignedChar(&animationByteOffset); // led number
            //   Serial.print(ledNum, DEC);

            unsigned char frameType = readByteUnsignedChar(
                    &animationByteOffset);

            switch (frameType) {
            case FT_FUNCTION:
                iValueAxisData[i][iValueAxisOffset + frame] =
                        readByteUnsignedChar(&animationByteOffset);
                //if(iValueAxisData[i][iValueAxisOffset + frame] != 0) {
                //  Serial.print("function is ");
                //  Serial.print(iValueAxisData[i][iValueAxisOffset + frame], DEC);
                //  Serial.print("\n");
                // }
                break;
            case FT_LINKED:
                iValueAxisData[i][iValueAxisOffset + frame] = 255;
                break;
            }
        }
    }
}

void readAxisData(int number) {
    Serial.print("readAxisData");
    Serial.print("\n");

    iTopOfTimeAxis = animationByteOffset;
    for (int frame = iTimeAxisLowValue; frame <= iTimeAxisHighValue; frame++) {
        // Serial.print(frame, DEC);
        // Serial.print("\n");
        //int ledNum = kInitialLed;
        for (int i = 0; i < numLeds; i++) {
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

        //Serial.print("SHOW!");
        leds.show();
        //Serial.print("DONE!");

        delayMicroseconds(iTimeAxisSpeed * MICROSECONDS_IN_A_MILLISECOND);
    }

    animationByteOffset = iTopOfTimeAxis;
}

unsigned char readByteUnsignedChar(int* aPosition) {
    unsigned char readByte = pgm_read_byte_near(animation1 + (*(aPosition))++);
    if (readByte == ESCAPE_BYTE) {
        readByte = pgm_read_byte_near(animation1 + (*(aPosition))++);
        readByte = readByte ^ XOR_BYTE;
    }

    return readByte;
}

signed char readByteSignedChar(int* aPosition) {
    signed char readByte = pgm_read_byte_near(animation1 + (*(aPosition))++);
    if (readByte == ESCAPE_BYTE) {
        readByte = pgm_read_byte_near(animation1 + (*(aPosition))++);
        readByte = readByte ^ XOR_BYTE;
    }

    return readByte;
}

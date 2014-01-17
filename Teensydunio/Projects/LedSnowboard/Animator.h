/*
 * animation.h
 *
 *  Created on: 14 Jan 2014
 *      Author: hydra
 */

#ifndef ANIMATOR_H_
#define ANIMATOR_H_

#include "Animations.h"

#define COLOR_COMPONENT_COUNT 3 // R,G and B

struct valueAxis {
    int lowValue;
    int highValue;
    int zeroValue;
} typedef ValueAxis;

// Frame Types
#define FT_COLOUR 1
#define FT_FUNCTION 2
#define FT_LINKED 3

#define INITIAL_LED 1

#define HEADER_BYTE 0x56
#define TERMINATING_BYTE 0x45
#define ESCAPE_BYTE 0x02
#define XOR_BYTE 0x20

class Animator {
public:
    void readAnimationDetails(void);
    void renderNextFrame(void);

    int iTimeAxisSpeed;
    
private:
    int animationByteOffset;
    int timeAxisNum;
    int valueAxisCount;
    int ledCount;

    int animationByteOffsetOfFirstFrame;

    int iNumFunctions;

    signed char valueAxisLowValue;
    signed char valueAxisHighValue;
    signed char valueAxisZeroValue;

    int valueAxisOffset;

    int timeAxisLowValue;
    int timeAxisHighValue;

    bool iBackgroundColour;
    unsigned char iBackgroundColourRed;
    unsigned char iBackgroundColourGreen;
    unsigned char iBackgroundColourBlue;

    unsigned char iValueAxisData[COUNT_OF_LEDS_IN_ANIMATION][MAX_VALUES_IN_RANGE_USED_BY_ANIMATION];
    signed int iFunctions[COUNT_OF_FUNCTIONS_IN_ANIMATION][COLOR_COMPONENT_COUNT];

    unsigned int frameIndex;

    void initializeFunctionData(unsigned int functionCount, unsigned int colorComponentCount);
    void initializeValueAxisData(unsigned int ledsInAnimation, unsigned int valuesInRange);
    
    void readAndSetColour(int ledNum);
    void readFunctionData(int num);
    void readTimeAxis(void);
    void readValueAxis(unsigned int valueAxisIndex);
    void processFrame(unsigned int frameIndex);
    void beginReadAxis(void);
};

#endif /* ANIMATOR_H_ */

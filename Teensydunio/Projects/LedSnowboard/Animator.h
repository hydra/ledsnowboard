/*
 * animation.h
 *
 *  Created on: 14 Jan 2014
 *      Author: hydra
 */

#ifndef ANIMATOR_H_
#define ANIMATOR_H_

#include "Animations.h"

#include "File/FileReader.h"

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
    Animator(void);
    void readAnimationDetails(FileReader *fileReader);
    void renderNextFrame(void);
    bool haveAnimation(void);
    void reset(void);

    uint16_t timeAxisFrequencyMillis;
    
private:
    bool hasAnimation;
    FileReader *fileReader;
    
    uint32_t animationByteOffset;
    uint32_t animationByteOffsetOfFirstFrame;

    uint8_t valueAxisCount;
    uint16_t ledCount;
    uint8_t functionCount;

    int8_t valueAxisLowValue;
    int8_t valueAxisHighValue;
    int8_t valueAxisCentreValue;

    uint16_t valueAxisOffset;

    uint8_t timeAxisLowValue;
    uint8_t timeAxisHighValue;
    uint8_t frameIndex;

    bool hasBackgroundColour;
    uint8_t backgroundColourRed;
    uint8_t backgroundColourGreen;
    uint8_t backgroundColourBlue;

    uint8_t valueAxisFunctionIndexes[COUNT_OF_LEDS_IN_ANIMATION][MAX_VALUES_IN_RANGE_USED_BY_ANIMATION];
    int32_t functionData[COUNT_OF_FUNCTIONS_IN_ANIMATION][COLOR_COMPONENT_COUNT];

    void initializeFunctionData(uint8_t functionCount, uint8_t colorComponentCount);
    void initializeValueAxisData(uint16_t ledsInAnimation, uint16_t valuesInRange);
    
    void readAndSetColour(uint16_t ledIndex);
    void readFunctionData(uint8_t functionIndex);
    void readTimeAxisHeader(void);
    void readValueAxis(uint8_t valueAxisIndex);
    void processFrame(uint8_t frameIndex);
    void beginReadAxisHeader(void);
    
    uint32_t readUnsignedInt32(void);
    uint8_t readUnsignedByte(uint32_t* aPosition);
    int8_t readSignedByte(uint32_t* aPosition);

};

#endif /* ANIMATOR_H_ */

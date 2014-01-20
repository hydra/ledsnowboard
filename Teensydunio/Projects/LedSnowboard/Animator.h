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
#include "ValueAxis.h"

#define COLOR_COMPONENT_COUNT 3 // R,G and B

// Frame Types
#define FT_COLOUR 1
#define FT_FUNCTION 2
#define FT_LINKED 3

#define INITIAL_LED 1

#define HEADER_BYTE 0x56
#define TERMINATING_BYTE 0x45

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

    uint16_t valueAxisOffset;

    uint8_t timeAxisLowValue;
    uint8_t timeAxisHighValue;
    uint8_t frameIndex;

    bool hasBackgroundColour;
    uint8_t backgroundColourRed;
    uint8_t backgroundColourGreen;
    uint8_t backgroundColourBlue;

    ValueAxis **valueAxes;

    int32_t functionData[COUNT_OF_FUNCTIONS_IN_ANIMATION][COLOR_COMPONENT_COUNT];
    void initializeFunctionData(uint8_t functionCount, uint8_t colorComponentCount);

    void allocateFunctionIndices(ValueAxis *valueAxis);
    void initializeFunctionIndices(ValueAxis *valueAxis);
    void readFunctionIndices(ValueAxis *valueAxis);
    
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

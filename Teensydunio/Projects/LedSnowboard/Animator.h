/*
 * animation.h
 *
 *  Created on: 14 Jan 2014
 *      Author: hydra
 */

#ifndef ANIMATOR_H_
#define ANIMATOR_H_

#define COLOR_COMPONENT_COUNT 3 // R,G and B

#define INITIAL_LED 1

#define HEADER_BYTE 0x56
#define TERMINATING_BYTE 0x45

class FileReader;
class AnimationReader;
class ValueAxis;
class ValueAxisSource;

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
    AnimationReader *animationReader;
    
    uint32_t animationByteOffsetOfFirstFrame;

    uint8_t valueAxisCount;
    uint16_t ledCount;
    uint8_t functionCount;

    uint8_t timeAxisLowValue;
    uint8_t timeAxisHighValue;
    uint8_t frameIndex;

    bool hasBackgroundColour;
    uint8_t backgroundColourRed;
    uint8_t backgroundColourGreen;
    uint8_t backgroundColourBlue;

    ValueAxis **valueAxes;
    ValueAxisSource **valueAxisSources;

    int32_t **functionData;
    void allocateFunctionData(void);
    void initializeFunctionData(uint8_t colorComponentCount);

    void allocateFunctionIndices(ValueAxis *valueAxis);
    void initializeFunctionIndices(ValueAxis *valueAxis);
    void readFunctionIndices(ValueAxis *valueAxis);
    
    void readAndSetColour(uint16_t ledIndex);
    void readFunctionData(uint8_t functionIndex);
    void readTimeAxisHeader(void);
    void readBackgroundColour(void);
    void readValueAxis(uint8_t valueAxisIndex);
    void calculateValueAxisPositions(void);
    void processFrame(uint8_t frameIndex);
    void beginReadAxisHeader(void);

    int8_t determineValueAxisPosition(ValueAxis *valueAxis, uint8_t valueAxisIndex);
};

#endif /* ANIMATOR_H_ */

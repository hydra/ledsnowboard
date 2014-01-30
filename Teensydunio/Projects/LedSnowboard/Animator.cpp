/*
 * animation.c
 *
 *  Created on: 14 Jan 2014
 *      Author: hydra
 */

#include "WProgram.h"

#include <new.cpp>

#include "Config.h"

#include "System.h"
#include "Time.h"

#include "AccelGyro.h"
#include <OctoWS2811.h>

#include "File/FileReader.h"
#include "AnimationReader.h"
#include "LedFunctionRanges.h"

#include "ValueAxis.h"
#include "ValueAxisSource.h"

#include "Animator.h"

#include "FrameType.h"
#include "ScaleMath.h"

extern OctoWS2811 leds;
extern AccelGyro accelGyro;

#define SOURCE_COUNT 2

Animator::Animator(void) :
        timeAxisFrequencyMillis(0),
        hasAnimation(false),
        fileReader(NULL),
        animationReader(NULL),
        animationByteOffsetOfFirstFrame(0),
        valueAxisCount(0),
        ledCount(0),
        functionCount(0),
        timeAxisLowValue(0),
        timeAxisHighValue(0),
        frameIndex(0),
        hasBackgroundColour(false),
        backgroundColourRed(0),
        backgroundColourGreen(0),
        backgroundColourBlue(0),
        valueAxes(NULL),
        valueAxisSources(NULL),
        functionData(NULL) {
}

bool Animator::haveAnimation(void) {
    return hasAnimation;
}

void Animator::reset(void) {
    hasAnimation = false;

    if (functionData) {
#ifdef USE_MULTIPLE_MALLOC_CALLS_FOR_MULTIDIMENSIONAL_ARRAYS
        for (uint8_t functionIndex = 0; functionIndex < functionCount; functionIndex++) {
            free(functionData[functionIndex]);
        }
#endif
        free(functionData);
        functionData = NULL;
    }

    if (valueAxes) {
        for(uint8_t valueAxisIndex = 0; valueAxisIndex < valueAxisCount; valueAxisIndex++) {
            ValueAxis *valueAxis = valueAxes[valueAxisIndex];
            if (!valueAxis) {
                continue;
            }
            delete valueAxis;
            
            valueAxes[valueAxisIndex] = 0;
        }
        free(valueAxes);
        valueAxes = NULL;
    }

    if (valueAxisSources) {
        for (uint8_t valueAxisSourceIndex = 0; valueAxisSourceIndex < SOURCE_COUNT;
                valueAxisSourceIndex++) {
            ValueAxisSource *valueAxisSource = valueAxisSources[valueAxisSourceIndex];
            if (!valueAxisSource) {
                continue;
            }
            delete valueAxisSource;
        }
        free(valueAxisSources);
        valueAxisSources = NULL;
    }

    animationByteOffsetOfFirstFrame = 0;

    valueAxisCount = 0;
    ledCount = 0;
    functionCount = 0;

    timeAxisLowValue = 0;
    timeAxisHighValue = 0;
    frameIndex = 0;

    hasBackgroundColour = false;
    backgroundColourRed = 0;
    backgroundColourGreen = 0;
    backgroundColourBlue = 0;
    
    if (animationReader) {
        delete animationReader;
        animationReader = NULL;
    }
}


void Animator::initializeFunctionData(uint8_t colorComponentCount) {
    for (uint8_t i = 0; i < functionCount; i++) {
#ifdef DEBUG_INITIALIZATION_OF_FUNCTION_DATA
        Serial.print("Row: ");
        Serial.print(i, DEC);
        Serial.print(" - ");
#endif
        for (uint8_t j = 0; j < colorComponentCount; j++) {
            functionData[i][j] = 0;
#ifdef DEBUG_INITIALIZATION_OF_DATA
            Serial.print("#");
#endif
        }
#ifdef DEBUG_INITIALIZATION_OF_DATA
        Serial.println("..OK");
#endif
    }
    Serial.println("functionData initialised");
}

void Animator::readAnimationDetails(FileReader *_fileReader) {
    fileReader = _fileReader;
    animationReader = new AnimationReader(fileReader);

    animationReader->readUnsignedByte(); // 'L'
	animationReader->readUnsignedByte(); // 'A'
	animationReader->readUnsignedByte(); // 'A'
	animationReader->readUnsignedByte(); // 'N'

    animationReader->readUnsignedByte(); // highVersion
    animationReader->readUnsignedByte(); // lowVersion

    uint8_t ledCountHigh = animationReader->readUnsignedByte();
    uint8_t ledCountLow = animationReader->readUnsignedByte();

    ledCount = ledCountHigh |= ledCountLow << 8;
    Serial.print("led count is ");
    Serial.print(ledCount, DEC);
    Serial.println();

    functionCount = animationReader->readUnsignedByte();
    Serial.print("function count is ");
    Serial.print(functionCount, DEC);
    Serial.println();

    allocateFunctionData();
    initializeFunctionData(COLOR_COMPONENT_COUNT);

    for (uint8_t functionIndex = 0; functionIndex < functionCount; functionIndex++) {
        readFunctionData(functionIndex);
    }


    valueAxisCount = animationReader->readUnsignedByte();
    Serial.print("value axis count is ");
    Serial.print(valueAxisCount, DEC);
    Serial.println();

    size_t memoryToAllocate = valueAxisCount * sizeof(ValueAxis *);
    Serial.print("memoryToAllocate: ");
    Serial.print(memoryToAllocate, DEC);
    Serial.println();

    valueAxes = (ValueAxis **)malloc(memoryToAllocate);
    verifyMemoryAllocation((void *)valueAxes);
    showFreeRam();

    readTimeAxisHeader();

    for (uint8_t valueAxisIndex = 0; valueAxisIndex < valueAxisCount;
            valueAxisIndex++) {
        ValueAxis *valueAxis = new ValueAxis(ledCount, animationReader);
        valueAxes[valueAxisIndex] = valueAxis;
        readValueAxis(valueAxisIndex);
    }

    memoryToAllocate = SOURCE_COUNT * sizeof(ValueAxisSource *);
    Serial.print("memoryToAllocate: ");
    Serial.print(memoryToAllocate, DEC);
    Serial.println();

    valueAxisSources = (ValueAxisSource **)malloc(memoryToAllocate);
    verifyMemoryAllocation((void *)valueAxisSources);
    showFreeRam();

    for (uint8_t valueAxisSourceIndex = 0; valueAxisSourceIndex < SOURCE_COUNT;
            valueAxisSourceIndex++) {
        ValueAxisSource *valueAxisSource = new ValueAxisSource(valueAxes, valueAxisCount);
        valueAxisSources[valueAxisSourceIndex] = valueAxisSource;
    }

    animationByteOffsetOfFirstFrame = animationReader->getPosition();

    frameIndex = timeAxisLowValue;

    hasAnimation = true;
}

void Animator::allocateFunctionData(void) {
    const size_t row_pointers_bytes = functionCount * sizeof *functionData;
    const size_t row_elements_bytes = COLOR_COMPONENT_COUNT * sizeof(int32_t);
    const size_t memoryToAllocate = row_pointers_bytes + (functionCount * row_elements_bytes);

    Serial.print("memoryToAllocate (rows, row pointer size, columns*row element size, total): ");
    Serial.print(functionCount, DEC);
    Serial.print(", ");
    Serial.print(row_pointers_bytes, DEC);
    Serial.print(", ");
    Serial.print(COLOR_COMPONENT_COUNT, DEC);
    Serial.print("*");
    Serial.print(row_elements_bytes, DEC);
    Serial.print(", ");
    Serial.print(memoryToAllocate, DEC);
    Serial.println();

#ifdef USE_MULTIPLE_MALLOC_CALLS_FOR_MULTIDIMENSIONAL_ARRAYS
    Serial.print("memoryToAllocate (row pointers, row elements): (");
    Serial.print(row_pointers_bytes, DEC);
    Serial.print(", ");
    Serial.print(row_elements_bytes, DEC);
    Serial.println(")");

    Serial.print("Allocating function data...");
    functionData = (int32_t **) malloc(row_pointers_bytes);
    verifyMemoryAllocation((void *)functionData);
    Serial.println("OK");

    Serial.print("Allocating function data rows: ");
    for(size_t i = 0; i < functionCount; i++) {
        functionData[i] = (int32_t *) malloc(row_elements_bytes);
        verifyMemoryAllocation((void *)functionData[i]);
        Serial.print("#");
    }
    Serial.println(" OK");
#else
    // FIXME is this really correct?

    functionData = (int32_t **) malloc(memoryToAllocate);

    Serial.print("functionData: ");
    Serial.print((uint32_t)functionData, HEX);
    Serial.println();

    if (!functionData) {
        return;
    }

    int32_t *data = (int32_t *)functionData + functionCount;

    for(size_t i = 0; i < functionCount; i++) {
        functionData[i] = data + i * COLOR_COMPONENT_COUNT;

        Serial.print("functionData[");
        Serial.print(i, DEC);
        Serial.print("]: ");
        Serial.print((uint32_t)functionData[i], HEX);
        Serial.println();
    }
#endif
    Serial.println("functionData allocated");
    showFreeRam();
}

void Animator::readFunctionData(uint8_t functionIndex) {

    int32_t redIncrement = animationReader->readUnsignedInt32();
    int32_t greenIncrement = animationReader->readUnsignedInt32();
    int32_t blueIncrement = animationReader->readUnsignedInt32();

    Serial.print("Increments (r,g,b):");
    Serial.print(redIncrement, DEC);
    Serial.print(", ");
    Serial.print(greenIncrement, DEC);
    Serial.print(", ");
    Serial.print(blueIncrement, DEC);
    Serial.println();

    functionData[functionIndex][0] = redIncrement;
    functionData[functionIndex][1] = greenIncrement;
    functionData[functionIndex][2] = blueIncrement;
}

int32_t fixIncrement(int32_t increment) {
    if (increment < -0xFFFF) {
    	increment = -0xFFFF;
    }

    if (increment > 0xFFFF) {
    	increment = 0xFFFF;
    }
    return increment;
}

uint8_t applyIncrement(uint8_t colour, int32_t increment) {
    int32_t colourBig = colour * 0x100;

    colourBig += increment;

    if (colourBig < 0) {
        return 0;
    }

    if (colourBig >= (0xFF * 0x100)) {
        return 0xFF;
    }
    uint8_t colourSmall = colourBig / 0x100;
    return colourSmall;
}

int8_t Animator::determineValueAxisPosition(ValueAxis *valueAxis, uint8_t valueAxisIndex) {

    uint8_t sourceIndex = valueAxisIndex & 1;
    return valueAxisSources[sourceIndex]->retrievePosition(valueAxisIndex);
}

void Animator::readAndSetColour(uint16_t ledIndex) {
    
    uint8_t red = animationReader->readUnsignedByte();
    uint8_t green = animationReader->readUnsignedByte();
    uint8_t blue = animationReader->readUnsignedByte();


#ifdef DEBUG_ANIMATOR_CODEC_LED_COLOURS
    if (DEBUG_LED_INDEX_TEST) {
        Serial.print("led colors (ledIndex) (r,g,b): (");
        Serial.print(ledIndex, DEC);
        Serial.print(") (0x");
        Serial.print(red, HEX);
        Serial.print(",0x");
        Serial.print(green, HEX);
        Serial.print(",0x");
        Serial.print(blue, HEX);
        Serial.println(")");
    }
#endif


    if (hasBackgroundColour &&
    	(
    		red == backgroundColourRed &&
    		green == backgroundColourGreen &&
    		blue == backgroundColourBlue
    	)
    ) {
        leds.setPixel(ledIndex, red, green, blue);
        return;
    }

    int32_t redIncrement = 0;
    int32_t greenIncrement = 0;
    int32_t blueIncrement = 0;

    for (uint8_t valueAxisIndex = 0; valueAxisIndex < valueAxisCount; valueAxisIndex++) {
        ValueAxis *currentValueAxis = valueAxes[valueAxisIndex];

        int8_t valueAxisPosition = determineValueAxisPosition(currentValueAxis, valueAxisIndex);

        int8_t start = 0;
        int8_t end = 0;
        if (valueAxisPosition < 0) {
            start = valueAxisPosition;
            end = currentValueAxis->valueAxisCentreValue;
        } else if (valueAxisPosition > 0) {
            start = currentValueAxis->valueAxisCentreValue + 1;
            end = valueAxisPosition + 1;
        }

#ifdef DEBUG_ANIMATOR_CODEC_VALUE_AXIS
        if (DEBUG_LED_INDEX_TEST) {
            Serial.print("led valueAxis (index,position) (start,end): (");
            Serial.print(valueAxisIndex, DEC);
            Serial.print(",");
            Serial.print(valueAxisPosition, DEC);
            Serial.print(") (");
            Serial.print(start, DEC);
            Serial.print(",");
            Serial.print(end, DEC);
            Serial.println(")");

            Serial.print("led increments (functionIndex) (r,g,b): ");
        }
#endif

        for (int8_t valueAxisValue = start; valueAxisValue < end; valueAxisValue++) {

            uint8_t functionIndex = currentValueAxis->retrieveFunctionIndex(ledIndex, valueAxisValue);

            redIncrement += functionData[functionIndex][0];
            greenIncrement += functionData[functionIndex][1];
            blueIncrement += functionData[functionIndex][2];

#ifdef DEBUG_ANIMATOR_CODEC_VALUE_AXIS
            if (DEBUG_LED_INDEX_TEST) {
                if (valueAxisValue != start) {
                    Serial.print(", ");
                }
                Serial.print("(");
                Serial.print(functionIndex, DEC);
                Serial.print(") (");
                Serial.print(redIncrement, DEC);
                Serial.print(",");
                Serial.print(greenIncrement, DEC);
                Serial.print(",");
                Serial.print(blueIncrement, DEC);
                Serial.print(")");
            }
#endif
        }

#ifdef DEBUG_ANIMATOR_CODEC_VALUE_AXIS
        if (DEBUG_LED_INDEX_TEST) {
            Serial.println();
        }
#endif

#ifdef DEBUG_ANIMATOR_CODEC_FINAL_INCREMENTS
        if (DEBUG_LED_INDEX_TEST) {
            Serial.print("led pre-fixed increments (r,g,b): (");

            Serial.print(redIncrement, DEC);
            Serial.print(",");
            Serial.print(greenIncrement, DEC);
            Serial.print(",");
            Serial.print(blueIncrement, DEC);
            Serial.println(")");
        }
#endif

        redIncrement = fixIncrement(redIncrement);
        greenIncrement = fixIncrement(greenIncrement);
        blueIncrement = fixIncrement(blueIncrement);

#ifdef DEBUG_ANIMATOR_CODEC_FINAL_INCREMENTS
        if (DEBUG_LED_INDEX_TEST) {
            Serial.print("led post-fixed increments (r,g,b): (");

            Serial.print(redIncrement, DEC);
            Serial.print(",");
            Serial.print(greenIncrement, DEC);
            Serial.print(",");
            Serial.print(blueIncrement, DEC);
            Serial.println(")");
        }
#endif
    }

    red = applyIncrement(red, redIncrement);
    green = applyIncrement(green, greenIncrement);
    blue = applyIncrement(blue, blueIncrement);

#ifdef DEBUG_ANIMATOR_CODEC_LED_COLOURS
    if (DEBUG_LED_INDEX_TEST) {
        Serial.print("led final color: (ledIndex) (r,g,b): (");
        Serial.print(ledIndex, DEC);
        Serial.print(") (0x");
        Serial.print(red, HEX);
        Serial.print(",0x");
        Serial.print(green, HEX);
        Serial.print(",0x");
        Serial.print(blue, HEX);
        Serial.println(")");
    }
#endif

#ifdef APPLY_BRIGHTNESS_HACK
    red = scaleRange(red, 0x00, 0xff, 0x00, BRIGHTNESS_MAX);
    green = scaleRange(green, 0x00, 0xff, 0x00, BRIGHTNESS_MAX);
    blue = scaleRange(blue, 0x00, 0xff, 0x00, BRIGHTNESS_MAX);
#endif
    
    leds.setPixel(ledIndex, red, green, blue);
}

void Animator::beginReadAxisHeader(void) {
	uint8_t axisType = animationReader->readUnsignedByte();
	uint8_t priority = animationReader->readUnsignedByte();
    bool opaque = animationReader->readUnsignedByte();
}

void Animator::readTimeAxisHeader(void) {
    beginReadAxisHeader();

    timeAxisLowValue = animationReader->readUnsignedByte();
    Serial.print("time axis low value : ");
    Serial.print(timeAxisLowValue, DEC);
    Serial.println();
    timeAxisHighValue = animationReader->readUnsignedByte();
    Serial.print("time axis high value : ");
    Serial.print(timeAxisHighValue, DEC);
    Serial.println();
    timeAxisFrequencyMillis = animationReader->readUnsignedByte();  // FIXME should be uint16_t
    Serial.print("time axis speed : ");
    Serial.print(timeAxisFrequencyMillis, DEC);
    Serial.println();

    readBackgroundColour();
}

void Animator::readBackgroundColour(void) {
    hasBackgroundColour = animationReader->readUnsignedByte();
    if (!hasBackgroundColour) {
        Serial.println("No background colour detected.");
        return;
    }

    backgroundColourRed = animationReader->readUnsignedByte();
    backgroundColourGreen = animationReader->readUnsignedByte();
    backgroundColourBlue = animationReader->readUnsignedByte();

    Serial.print("Background colour : ");
    Serial.print(backgroundColourRed, HEX);
    Serial.print(" ");
    Serial.print(backgroundColourGreen, HEX);
    Serial.print(" ");
    Serial.print(backgroundColourBlue, HEX);
    Serial.println();
}

void Animator::readValueAxis(uint8_t valueAxisIndex) {
    
    beginReadAxisHeader();

    ValueAxis *valueAxis = valueAxes[valueAxisIndex];

    valueAxis->initialise();
}

void Animator::calculateValueAxisPositionsForEachSource(void) {
    if (valueAxisCount == 0) {
        return;
    }

    for (uint8_t valueAxisSourceIndex = 0; valueAxisSourceIndex < SOURCE_COUNT; valueAxisSourceIndex++) {
        ValueAxisSource *valueAxisSource = valueAxisSources[valueAxisSourceIndex];

        AccelerationData *sample = accelGyro.getLatestSample();

        int16_t rawValue;
        if (valueAxisSourceIndex & 1) {
            rawValue = sample->y;
        } else {
            rawValue = sample->x;
        }

        calculateValueAxisPositionsForSource(valueAxisSource, rawValue, ACCEL_RANGE_MIN, ACCEL_RANGE_MAX);
    }
}

void Animator::calculateValueAxisPositionsForSource(ValueAxisSource *valueAxisSource, int16_t rawValue, int16_t rangeMin, int16_t rangeMax) {


#ifdef DEBUG_VALUE_AXIS_POSITIONS
    Serial.print("Value Axis Positions (index,low,high) (value,position): ");
#endif

    int16_t clampedValue = max(rangeMin, min(rangeMax, rawValue));

    for (uint8_t valueAxisIndex = 0; valueAxisIndex < valueAxisCount; valueAxisIndex++) {
        ValueAxis *valueAxis = valueAxes[valueAxisIndex];

        int8_t position = scaleRange(clampedValue, rangeMin, rangeMax, valueAxis->valueAxisLowValue, valueAxis->valueAxisHighValue);
        valueAxisSource->applyValueAxisPosition(valueAxisIndex, position);

#ifdef DEBUG_VALUE_AXIS_POSITIONS
        if (valueAxisIndex != 0) {
            Serial.print(", ");
        }
        Serial.print("(");
        Serial.print(valueAxisIndex, DEC);
        Serial.print(",");
        Serial.print(valueAxis->valueAxisLowValue, DEC);
        Serial.print(",");
        Serial.print(valueAxis->valueAxisLowValue, DEC);
        Serial.print(") (");
        Serial.print(value, DEC);
        Serial.print(",");
        Serial.print(valueAxisPositions[valueAxisIndex], DEC);
        Serial.print(")");
#endif
    }
#ifdef DEBUG_VALUE_AXIS_POSITIONS
    Serial.println();
#endif
}

void Animator::renderNextFrame() {

    calculateValueAxisPositionsForEachSource();

    processFrame(frameIndex);

    frameIndex++;
    
    if (frameIndex > timeAxisHighValue) {
        uint8_t terminatingByte = animationReader->readUnsignedByte();
        if (terminatingByte != TERMINATING_BYTE) {
            Serial.println("Missing EOF marker");

            systemHalt(); // FIXME just reset the animation instead.
        }
        
        // rewind after last frame
        frameIndex = timeAxisLowValue;
        animationReader->seek(animationByteOffsetOfFirstFrame);
    }
}

void Animator::processFrame(uint8_t frameIndex) {
#ifdef DEBUG_ANIMATOR_FRAME_POSITIONS
    uint32_t position = animationReader->getPosition();
    Serial.print("Reader position before frame: 0x");
    Serial.println(position, HEX);
#endif

#ifdef DEBUG_ANIMATOR_FRAME
    Serial.print("Frame: ");
    Serial.println(frameIndex);

    Serial.print("Frame data (ledIndex,type): ");
#endif
    for (uint16_t ledIndex = 0; ledIndex < ledCount; ledIndex++) {

        uint8_t frameType = animationReader->readUnsignedByte();
#ifdef DEBUG_ANIMATOR_FRAME
        Serial.print("(0x");
        Serial.print(ledIndex, HEX);
        Serial.print(",0x");
        Serial.print(frameType, HEX);
        Serial.print(")");
#endif
        switch (frameType) {
            case FT_FUNCTION:
                //readFunctionAndSetColour();
                break;
            case FT_COLOUR:
                readAndSetColour(ledIndex);
                break;
        }
    }
#ifdef DEBUG_ANIMATOR_FRAME
    Serial.println();
#endif

    #ifdef DEBUG_ANIMATOR_FRAME_POSITIONS
    position = animationReader->getPosition();
    Serial.print("Reader position after frame: 0x");
    Serial.println(position, HEX);
#endif
}


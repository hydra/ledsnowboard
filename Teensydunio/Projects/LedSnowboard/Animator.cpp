/*
 * animation.c
 *
 *  Created on: 14 Jan 2014
 *      Author: hydra
 */

#include "WProgram.h"

#include <new.cpp>

#include "System.h"
#include "Time.h"

#include "AccelGyro.h"
#include <OctoWS2811.h>

#include "Animator.h"

#include "Animations.h"

extern OctoWS2811 leds;
extern AccelGyro accelGyro;

static int8_t accelerometerXValue;
static int8_t accelerometerYValue;

Animator::Animator(void) :
        hasAnimation(false),
        valueAxes(NULL) {
}

bool Animator::haveAnimation(void) {
    return hasAnimation;
}

void Animator::reset(void) {
    hasAnimation = false;

    if (functionData) {
#ifdef USE_MULTIPLE_MALLOC_CALLS_FOR_MULTIDIMENSIONAL_ARRAYS
        for (uint8_t functionIndex; functionIndex < functionCount; functionIndex++) {
            free(functionData[functionIndex]);
        }
#endif
        free(functionData);
        functionData = 0;
    }

    if (valueAxes) {
        for(uint8_t valueAxisIndex = 0; valueAxisIndex < valueAxisCount; valueAxisIndex++) {
            ValueAxis *valueAxis = valueAxes[valueAxisIndex];
            if (!valueAxis) {
                continue;
            }
#ifdef USE_MULTIPLE_MALLOC_CALLS_FOR_MULTIDIMENSIONAL_ARRAYS
            for (uint16_t functionIndicesEntryIndex = 0; functionIndicesEntryIndex < valueAxis->functionIndicesEntryCount; functionIndicesEntryIndex++) {
                free(valueAxis->functionIndices[functionIndicesEntryIndex]);
            }
#endif
            free(valueAxis->functionIndices);
            delete valueAxis;
            valueAxes[valueAxisIndex] = 0;
        }
        free(valueAxes);
        valueAxes = 0;
    }

    animationByteOffset = 0;
    animationByteOffsetOfFirstFrame = 0;

    valueAxisCount = 0;
    ledCount = 0;
    functionCount = 0;

    valueAxisOffset = 0;

    timeAxisLowValue = 0;
    timeAxisHighValue = 0;
    frameIndex = 0;

    hasBackgroundColour = false;
    backgroundColourRed = 0;
    backgroundColourGreen = 0;
    backgroundColourBlue = 0;
}

#ifdef USE_OLD_FILE_FORMAT_WITH_ESCAPING

#define ESCAPE_BYTE 0x02
#define XOR_BYTE 0x20

uint8_t Animator::readUnsignedByte(uint32_t* aPosition) {
    fileReader->seek(*aPosition);
    unsigned char readByte = (unsigned char)fileReader->readByte();
    (*(aPosition))++;

    if (readByte == ESCAPE_BYTE) {
        readByte = (unsigned char)fileReader->readByte();
        (*(aPosition))++;
        readByte = readByte ^ XOR_BYTE;
    }

    return readByte;
}

int8_t Animator::readSignedByte(uint32_t* aPosition) {
    fileReader->seek(*aPosition);
    signed char readByte = (signed char)fileReader->readByte();
    (*(aPosition))++;

    if (readByte == ESCAPE_BYTE) {
        readByte =  (signed char)fileReader->readByte();
        (*(aPosition))++;
        readByte = readByte ^ XOR_BYTE;
    }

    return readByte;
}
#else
uint8_t Animator::readUnsignedByte(uint32_t* aPosition) {
    fileReader->seek(*aPosition);
    unsigned char readByte = (unsigned char)fileReader->readByte();
    (*(aPosition))++;

    return readByte;
}

int8_t Animator::readSignedByte(uint32_t* aPosition) {
    fileReader->seek(*aPosition);
    signed char readByte = (signed char)fileReader->readByte();
    (*(aPosition))++;

    return readByte;
}
#endif

void Animator::initializeFunctionData(uint8_t colorComponentCount) {
    for (uint8_t i = 0; i < functionCount; i++) {
#ifdef DEBUG_INITIALIZATION_OF_DATA
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

void Animator::initializeFunctionIndices(ValueAxis *valueAxis) {
    for (uint16_t i = 0; i < ledCount; i++) {
#ifdef DEBUG_INITIALIZATION_OF_DATA
        Serial.print("Row: ");
        Serial.print(i, DEC);
        Serial.print(" - ");
#endif
        for (uint16_t j = 0; j < valueAxis->functionIndicesEntryCount; j++) {
#if 0
            Serial.print((uint32_t)&valueAxis->functionIndices[i], HEX);
#endif
            valueAxis->functionIndices[i][j] = 0;
#ifdef DEBUG_INITIALIZATION_OF_DATA
            Serial.print("#");
#endif
#if 0
            Serial.print(": ");
            Serial.print(valueAxis->functionIndices[i][j], HEX);
            Serial.println();
#endif
        }
#ifdef DEBUG_INITIALIZATION_OF_DATA
        Serial.println("..OK");
#endif
    }
    Serial.println("functionIndicies initialised");
}

void Animator::readAnimationDetails(FileReader *_fileReader) {
    fileReader = _fileReader;
    animationByteOffset = 0;

    if (readUnsignedByte(&animationByteOffset) != HEADER_BYTE) {
        //throw new InvalidAnimationException("No header byte");
    }

    uint8_t ledCountHigh = readUnsignedByte(&animationByteOffset);
    uint8_t ledCountLow = readUnsignedByte(&animationByteOffset);

    ledCount = ledCountHigh |= ledCountLow << 8;
    Serial.print("led count is ");
    Serial.print(ledCount, DEC);
    Serial.println();

    functionCount = readUnsignedByte(&animationByteOffset);
    Serial.print("function count is ");
    Serial.print(functionCount, DEC);
    Serial.println();

    allocateFunctionData();
    initializeFunctionData(COLOR_COMPONENT_COUNT);

    for (uint8_t functionIndex = 0; functionIndex < functionCount; functionIndex++) {
        readFunctionData(functionIndex);
    }

    valueAxisCount = readUnsignedByte(&animationByteOffset);
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
        ValueAxis *valueAxis = new ValueAxis();
        valueAxes[valueAxisIndex] = valueAxis;
        readValueAxis(valueAxisIndex);
    }
    
    animationByteOffsetOfFirstFrame = animationByteOffset;

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

uint32_t Animator::readUnsignedInt32(void) {

    uint8_t red1 = readUnsignedByte(&animationByteOffset);
    uint8_t red2 = readUnsignedByte(&animationByteOffset);
    uint8_t red3 = readUnsignedByte(&animationByteOffset);
    uint8_t red4 = readUnsignedByte(&animationByteOffset);

    uint32_t r = red1;
    r |= red2 << 8;
    r |= red3 << 16;
    r |= red4 << 24;

    return r;
}

void Animator::readFunctionData(uint8_t functionIndex) {

    int32_t redIncrement = readUnsignedInt32();
    int32_t greenIncrement = readUnsignedInt32();
    int32_t blueIncrement = readUnsignedInt32();

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

int32_t fixIncrement(int32_t unfixedIncrement) {
    if (unfixedIncrement < -65536) {
    	unfixedIncrement = 0;  // FIXME verify this is correct, should it be set to -65536?
    }

    if (unfixedIncrement > 65536) {
    	unfixedIncrement = 65536;
    }
    return unfixedIncrement;
}

uint8_t applyIncrement(uint8_t colour, int32_t increment) {
    uint32_t colourBig = colour * 256;

    colourBig += increment;
    if (colourBig < 0) {
        colourBig = 0;
    }

    if (colourBig > 65536) {
        colourBig = 65536;
    }
    colourBig = colourBig / 256;
    return colourBig;
}

int8_t determineValueAxisPosition(ValueAxis *valueAxis, uint8_t valueAxisIndex) {

    // chose input source
    if (valueAxisIndex & 0x1) {
        return accelerometerYValue;
    } else {
        return accelerometerXValue;
    }

    // TODO translate accelerometer readings into a range >= valueAxisLowValue && <= valueAxisHighValue
}

void Animator::readAndSetColour(uint16_t ledIndex) {
    
    uint8_t red = readUnsignedByte(&animationByteOffset);
    uint8_t green = readUnsignedByte(&animationByteOffset);
    uint8_t blue = readUnsignedByte(&animationByteOffset);

#ifdef DEBUG_ANIMATOR_CODEC
    Serial.print("ledIndex: ");
    Serial.print(ledIndex, DEC);
    Serial.print(": (r,g,b): ");
    Serial.print(red, HEX);
    Serial.print(", ");
    Serial.print(green, HEX);
    Serial.print(", ");
    Serial.print(blue, HEX);
    Serial.println();
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

        if (ledIndex == 0) {
            Serial.print("led 0 valueAxisIndex: ");
            Serial.print(valueAxisIndex, DEC);
            Serial.print(", valueAxisPosition: ");
            Serial.print(valueAxisPosition, DEC);
            Serial.println();
        }

        int8_t start = 0;
        int8_t end = 0;
        if (accelerometerValue < 0) {
            start = valueAxisPosition;
            end = currentValueAxis->valueAxisCentreValue;
        } else if (valueAxisPosition > 0) {
            start = currentValueAxis->valueAxisCentreValue + 1;
            end = valueAxisPosition + 1;
        }

        for (int8_t valueAxisValue = start; valueAxisValue < end; valueAxisValue++) {
            uint16_t valueAxisValueIndex = valueAxisOffset + valueAxisValue;

            int functionIndex = currentValueAxis->functionIndices[ledIndex][valueAxisValueIndex];

#ifdef DEBUG_ANIMATOR_CODEC
            Serial.print("valueAxisValueIndex: ");
            Serial.print(valueAxisValueIndex, DEC);
            Serial.print(", functionIndex:");
            Serial.print(functionIndex, DEC);
#endif

            redIncrement += functionData[functionIndex][0];
            greenIncrement += functionData[functionIndex][1];
            blueIncrement += functionData[functionIndex][2];
        }

#ifdef DEBUG_ANIMATOR_CODEC
        Serial.println();

        Serial.print("increments (r,g,b): ");

        Serial.print(redIncrement, DEC);
        Serial.print(", ");
        Serial.print(greenIncrement, DEC);
        Serial.print(", ");
        Serial.print(blueIncrement, DEC);
        Serial.println();
#endif

        redIncrement = fixIncrement(redIncrement);
        greenIncrement = fixIncrement(greenIncrement);
        blueIncrement = fixIncrement(blueIncrement);
    }

    red = applyIncrement(red, redIncrement);
    green = applyIncrement(green, greenIncrement);
    blue = applyIncrement(blue, blueIncrement);

    leds.setPixel(ledIndex, red, green, blue);
}

void Animator::beginReadAxisHeader(void) {
	uint8_t axisType = readUnsignedByte(&animationByteOffset);
	uint8_t priority = readUnsignedByte(&animationByteOffset);
    bool opaque = readUnsignedByte(&animationByteOffset);
}

void Animator::readTimeAxisHeader(void) {
    beginReadAxisHeader();

    timeAxisLowValue = readUnsignedByte(&animationByteOffset);
    Serial.print("time axis low value : ");
    Serial.print(timeAxisLowValue, DEC);
    Serial.println();
    timeAxisHighValue = readUnsignedByte(&animationByteOffset);
    Serial.print("time axis high value : ");
    Serial.print(timeAxisHighValue, DEC);
    Serial.println();
    timeAxisFrequencyMillis = readUnsignedByte(&animationByteOffset);  // FIXME should be uint16_t
    Serial.print("time axis speed : ");
    Serial.print(timeAxisFrequencyMillis, DEC);
    Serial.println();

    hasBackgroundColour = readUnsignedByte(&animationByteOffset);
    if (hasBackgroundColour) {
        Serial.print("background colour : ");
        backgroundColourRed = readUnsignedByte(&animationByteOffset);
        backgroundColourGreen = readUnsignedByte(&animationByteOffset);
        backgroundColourBlue = readUnsignedByte(&animationByteOffset);

        Serial.print(backgroundColourRed, HEX);
        Serial.print(" ");
        Serial.print(backgroundColourGreen, HEX);
        Serial.print(" ");
        Serial.print(backgroundColourBlue, HEX);
        Serial.println();
    }
}

void Animator::allocateFunctionIndices(ValueAxis *valueAxis) {
    valueAxis->functionIndicesEntryCount = -valueAxis->valueAxisLowValue + valueAxis->valueAxisHighValue;
    if (
        valueAxis->valueAxisCentreValue != valueAxis->valueAxisLowValue
        &&
        valueAxis->valueAxisCentreValue != valueAxis->valueAxisHighValue
    ) {
        valueAxis->functionIndicesEntryCount++;
    }

    Serial.print("functionIndicesEntryCount: ");
    Serial.print(valueAxis->functionIndicesEntryCount, DEC);
    Serial.println();

    const size_t row_pointers_bytes = ledCount * sizeof *valueAxis->functionIndices;
    const size_t row_elements_bytes = valueAxis->functionIndicesEntryCount * sizeof(uint8_t);
    const size_t memoryToAllocate = row_pointers_bytes + (ledCount * row_elements_bytes);

    Serial.print("memoryToAllocate (rows, row pointer size, columns*row element size, total): ");
    Serial.print(ledCount, DEC);
    Serial.print(", ");
    Serial.print(row_pointers_bytes, DEC);
    Serial.print(", ");
    Serial.print(valueAxis->functionIndicesEntryCount, DEC);
    Serial.print("*");
    Serial.print(row_elements_bytes, DEC);
    Serial.print(", ");
    Serial.print(memoryToAllocate, DEC);
    Serial.println();

#ifdef USE_MULTIPLE_MALLOC_CALLS_FOR_MULTIDIMENSIONAL_ARRAYS

    Serial.print("Allocating function indices...");
    valueAxis->functionIndices = (uint8_t **) malloc(row_pointers_bytes);
    verifyMemoryAllocation((void *)valueAxis->functionIndices);
    Serial.println("OK");

    Serial.print("Allocating function indices rows: ");
    for(size_t i = 0; i < ledCount; i++) {
        valueAxis->functionIndices[i] = (uint8_t *) malloc(row_elements_bytes);
        verifyMemoryAllocation((void *)valueAxis->functionIndices[i]);
        Serial.print("#");
    }
    Serial.println(" OK");
#else
    // FIXME is this really correct?

    valueAxis->functionIndices = (uint8_t **) malloc(memoryToAllocate);

    Serial.print("valueAxis->functionIndices: ");
    Serial.print((uint32_t)valueAxis->functionIndices, HEX);
    Serial.println();

    uint8_t *data = (uint8_t *)valueAxis->functionIndices + sizeof(uint8_t*) * ledCount;
    for(size_t i = 0; i < ledCount; i++) {
        valueAxis->functionIndices[i] = data + i * valueAxis->functionIndicesEntryCount;
        Serial.print("functionIndices[");
        Serial.print(i, DEC);
        Serial.print("]: ");
        Serial.print((uint32_t)valueAxis->functionIndices[i], HEX);
        Serial.println();
    }
#endif
    Serial.println("functionIndices allocated");
    showFreeRam();
}

void Animator::readValueAxis(uint8_t valueAxisIndex) {

    ValueAxis *valueAxis = valueAxes[valueAxisIndex];

    beginReadAxisHeader();

    valueAxis->valueAxisLowValue = readSignedByte(&animationByteOffset);
    Serial.print("value axis low value : ");
    Serial.print(valueAxis->valueAxisLowValue, DEC);
    Serial.println();
    valueAxis->valueAxisHighValue = readSignedByte(&animationByteOffset);
    Serial.print("value axis high value : ");
    Serial.print(valueAxis->valueAxisHighValue, DEC);
    Serial.println();
    valueAxis->valueAxisCentreValue = readSignedByte(&animationByteOffset);
    Serial.print("value axis zero value : ");
    Serial.print(valueAxis->valueAxisCentreValue, DEC);
    Serial.println();

    allocateFunctionIndices(valueAxis);

    initializeFunctionIndices(valueAxis);

    readFunctionIndices(valueAxis);
}

void Animator::readFunctionIndices(ValueAxis *valueAxis) {
    valueAxisOffset = -valueAxis->valueAxisLowValue;
    Serial.print("valueAxisOffset: ");
    Serial.print(valueAxisOffset, DEC);
    Serial.println();

    for (int8_t valueAxisValue = valueAxis->valueAxisLowValue; valueAxisValue <= valueAxis->valueAxisHighValue;
            valueAxisValue++) {

#if 0
        Serial.print("valueAxisValue: ");
        Serial.print(valueAxisValue, DEC);
        Serial.print(", ledChunkOffset: ");
        Serial.print(animationByteOffset, HEX);
        Serial.println();
#endif

        for (uint16_t ledIndex = 0; ledIndex < ledCount; ledIndex++) {
        	uint8_t frameType = readUnsignedByte(&animationByteOffset);

            uint16_t valueAxisIndex = valueAxisOffset + valueAxisValue;

            uint8_t functionIndex;

            switch (frameType) {
				case FT_FUNCTION:

					functionIndex = readUnsignedByte(&animationByteOffset);
					valueAxis->functionIndices[ledIndex][valueAxisIndex] = functionIndex;

#if 0
					if (valueAxis->functionIndices[ledIndex][valueAxisIndex] != 0) {
						Serial.print(", function: ");
						Serial.print(valueAxis->functionIndices[ledIndex][valueAxisIndex], DEC);
					}
#endif
				break;
				case FT_LINKED:
					functionIndex = 255;
					valueAxis->functionIndices[ledIndex][valueAxisIndex] = functionIndex;
				break;
            }
#if 0
            Serial.println();
#endif
        }
    }
}

void Animator::renderNextFrame() {

    accelGyro.refresh();

    accelerometerXValue = accelGyro.getNormalisedXValue();
    accelerometerYValue = accelGyro.getNormalisedYValue();

#if 0
    Serial.print("Processing frame: ");
    Serial.print(frameIndex, DEC);
    Serial.println();
#endif


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

void Animator::processFrame(uint8_t frameIndex) {
    for (uint16_t ledIndex = 0; ledIndex < ledCount; ledIndex++) {
        uint8_t frameType = readUnsignedByte(&animationByteOffset);

        switch (frameType) {
            case FT_FUNCTION:
                //readFunctionAndSetColour();
                break;
            case FT_COLOUR:
                readAndSetColour(ledIndex);
                break;
        }
    }
}


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

Animator::Animator(void) :
        hasAnimation(false) {
}

bool Animator::haveAnimation(void) {
    return hasAnimation;
}

void Animator::reset(void) {
    hasAnimation = false;

    animationByteOffset = 0;
    animationByteOffsetOfFirstFrame = 0;

    valueAxisCount = 0;
    ledCount = 0;
    functionCount = 0;

    // TODO freeValueAxes()
    free(valueAxes);

    valueAxisOffset = 0;

    timeAxisLowValue = 0;
    timeAxisHighValue = 0;
    frameIndex = 0;

    hasBackgroundColour = false;
    backgroundColourRed = 0;
    backgroundColourGreen = 0;
    backgroundColourBlue = 0;
}

#ifdef ANIMATION_IN_MEMORY
uint8_t Animator::readUnsignedByte(uint32_t* aPosition) {
    unsigned char readByte = (*(const unsigned char *)(animationData + (*(aPosition))++));
    if (readByte == ESCAPE_BYTE) {
        readByte = (*(const unsigned char *)(animationData + (*(aPosition))++));
        readByte = readByte ^ XOR_BYTE;
    }

    return readByte;
}

int8_t Animator::readSignedByte(uint32_t* aPosition) {
    signed char readByte = (*(const unsigned char *)(animationData + (*(aPosition))++));
    if (readByte == ESCAPE_BYTE) {
        readByte = (*(const unsigned char *)(animationData + (*(aPosition))++));
        readByte = readByte ^ XOR_BYTE;
    }

    return readByte;
}

#else

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
#endif // ANIMATION_IN_MEMORY

void Animator::initializeFunctionData(uint8_t functionCount, uint8_t colorComponentCount) {
    for (uint8_t i = 0; i < functionCount; i++) {
        for (uint8_t j = 0; j < colorComponentCount; j++) {
            functionData[i][j] = 0;
        }
    }
}

void Animator::initializeValueAxisData(valueAxis_t *valueAxis, uint16_t ledsInAnimation, uint16_t functionIndicesEntryCount) {
    for (uint16_t i = 0; i < ledsInAnimation; i++) {
        for (uint16_t j = 0; j < functionIndicesEntryCount; j++) {
#if 0
            Serial.print((uint32_t)&valueAxis->functionIndices[i], HEX);
            Serial.print(">");
#endif
            valueAxis->functionIndices[i][j] = 0;
#if 0
            Serial.print(": ");
            Serial.print(valueAxis->functionIndices[i][j], HEX);
            Serial.println();
#endif
        }
    }
}

void Animator::readAnimationDetails(FileReader *_fileReader) {
    fileReader = _fileReader;
    hasAnimation = true;
    animationByteOffset = 0;

    if (readUnsignedByte(&animationByteOffset) != HEADER_BYTE) {
        //throw new InvalidAnimationException("No header byte");
    }

    uint8_t ledCountHigh = readUnsignedByte(&animationByteOffset);
    uint8_t ledCountLow = readUnsignedByte(&animationByteOffset);

    ledCount = ledCountHigh |= ledCountLow << 8;
    Serial.print("led count is ");
    Serial.print(ledCount, DEC);
    Serial.print("\n");

    functionCount = readUnsignedByte(&animationByteOffset);
    Serial.print("function count is ");
    Serial.print(functionCount, DEC);
    Serial.print("\n");

    initializeFunctionData(functionCount, COLOR_COMPONENT_COUNT);

    for (uint8_t functionIndex = 0; functionIndex < functionCount; functionIndex++) {
        readFunctionData(functionIndex);
    }


    valueAxisCount = readUnsignedByte(&animationByteOffset);
    Serial.print("value axis count is ");
    Serial.print(valueAxisCount, DEC);
    Serial.print("\n");

    valueAxes = (valueAxis_t *) malloc(valueAxisCount * sizeof(valueAxis_t));

    readTimeAxisHeader();
    
    for (uint8_t valueAxisIndex = 0; valueAxisIndex < valueAxisCount;
            valueAxisIndex++) {
        readValueAxis(valueAxisIndex);
    }
    
    animationByteOffsetOfFirstFrame = animationByteOffset;

    frameIndex = timeAxisLowValue;
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
        valueAxis_t *currentValueAxis = &valueAxes[valueAxisIndex];

        int8_t accelerometerValue = accelGyro.getNormalisedAccelerometerXValue();

        int8_t start = 0;
        int8_t end = 0;
        if (accelerometerValue < 0) {
            start = accelerometerValue;
            end = currentValueAxis->valueAxisCentreValue;
        } else if (accelerometerValue > 0) {
            start = currentValueAxis->valueAxisCentreValue + 1;
            end = accelerometerValue + 1;
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
    Serial.print("\n");
    timeAxisHighValue = readUnsignedByte(&animationByteOffset);
    Serial.print("time axis high value : ");
    Serial.print(timeAxisHighValue, DEC);
    Serial.print("\n");
    timeAxisFrequencyMillis = readUnsignedByte(&animationByteOffset);  // FIXME should be uint16_t
    Serial.print("time axis speed : ");
    Serial.print(timeAxisFrequencyMillis, DEC);
    Serial.print("\n");

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
        Serial.print("\n");
    }
}

void Animator::readValueAxis(uint8_t valueAxisIndex) {

    valueAxis_t *valueAxis = &valueAxes[valueAxisIndex];

    beginReadAxisHeader();

    valueAxis->valueAxisLowValue = readSignedByte(&animationByteOffset);
    Serial.print("value axis low value : ");
    Serial.print(valueAxis->valueAxisLowValue, DEC);
    Serial.print("\n");
    valueAxis->valueAxisHighValue = readSignedByte(&animationByteOffset);
    Serial.print("value axis high value : ");
    Serial.print(valueAxis->valueAxisHighValue, DEC);
    Serial.print("\n");
    valueAxis->valueAxisCentreValue = readSignedByte(&animationByteOffset);
    Serial.print("value axis zero value : ");
    Serial.print(valueAxis->valueAxisCentreValue, DEC);
    Serial.print("\n");


    uint16_t functionIndicesEntryCount = -valueAxis->valueAxisLowValue + valueAxis->valueAxisHighValue;
    if (
        valueAxis->valueAxisCentreValue != valueAxis->valueAxisLowValue
        &&
        valueAxis->valueAxisCentreValue != valueAxis->valueAxisHighValue
    ) {
        functionIndicesEntryCount++;
    }

    Serial.print("functionIndicesEntryCount: ");
    Serial.print(functionIndicesEntryCount, DEC);
    Serial.print("\n");

    uint32_t size = functionIndicesEntryCount * ledCount;

    Serial.print("size: ");
    Serial.print(size, DEC);
    Serial.print("\n");

    const size_t row_pointers_bytes = ledCount * sizeof *valueAxis->functionIndices;
    const size_t row_elements_bytes = functionIndicesEntryCount * sizeof **valueAxis->functionIndices;
    const uint32_t memoryToAllocate = row_pointers_bytes + (ledCount * row_elements_bytes);

    Serial.print("memoryToAllocate: ");
    Serial.print(memoryToAllocate, DEC);
    Serial.print("\n");

    valueAxis->functionIndices = (uint8_t **) malloc(memoryToAllocate);

    Serial.print("valueAxis->functionIndices: ");
    Serial.print((uint32_t)valueAxis->functionIndices, HEX);
    Serial.print("\n");

    uint8_t *data = (uint8_t *)valueAxis->functionIndices + sizeof(uint8_t*) * ledCount;
    for(size_t i = 0; i < ledCount; i++) {
        valueAxis->functionIndices[i] = data + i * functionIndicesEntryCount;
    }

    initializeValueAxisData(valueAxis, ledCount, functionIndicesEntryCount);

    valueAxisOffset = -valueAxis->valueAxisLowValue;
    Serial.print("valueAxisOffset: ");
    Serial.print(valueAxisOffset, DEC);
    Serial.print("\n");

    for (int8_t valueAxisValue = valueAxis->valueAxisLowValue; valueAxisValue <= valueAxis->valueAxisHighValue;
            valueAxisValue++) {

        Serial.print("valueAxisValue: ");
        Serial.print(valueAxisValue, DEC);
        Serial.print(", ledChunkOffset: ");
        Serial.print(animationByteOffset, HEX);
        Serial.println();


        for (uint16_t ledIndex = 0; ledIndex < ledCount; ledIndex++) {

            uint16_t ledNumber = readUnsignedByte(&animationByteOffset); // led number // FIXME ledNum should be uint16_t
            uint8_t frameType = readUnsignedByte(&animationByteOffset);

            uint16_t valueAxisIndex = valueAxisOffset + valueAxisValue;

            Serial.print("ledNumber: ");
            Serial.print(ledNumber, DEC);
            Serial.print(", valueAxisIndex: ");
            Serial.print(valueAxisIndex, DEC);

            uint8_t functionIndex;

            switch (frameType) {
				case FT_FUNCTION:

					functionIndex = readUnsignedByte(&animationByteOffset);
					valueAxis->functionIndices[ledIndex][valueAxisIndex] = functionIndex;

					if (valueAxis->functionIndices[ledIndex][valueAxisIndex] != 0) {
						Serial.print(", function: ");
						Serial.print(valueAxis->functionIndices[ledIndex][valueAxisIndex], DEC);
					}
				break;
				case FT_LINKED:
					functionIndex = 255;
					valueAxis->functionIndices[ledIndex][valueAxisIndex] = functionIndex;
				break;
            }
            Serial.println();
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

void Animator::processFrame(uint8_t frameIndex) {
    for (uint16_t ledIndex = 0; ledIndex < ledCount; ledIndex++) {

        uint16_t ledNum = readUnsignedByte(&animationByteOffset); // led number // FIXME ledNum should be uint16_t
        //Serial.print(ledNum, DEC);
        //Serial.print("\n");

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


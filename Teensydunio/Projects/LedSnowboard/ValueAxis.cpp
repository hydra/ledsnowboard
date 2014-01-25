/*
 * ValueAxis.cpp
 *
 *  Created on: 20 Jan 2014
 *      Author: hydra
 */

#include "WProgram.h"

#include "Config.h"

#include "System.h"

#include "ValueAxis.h"
#include "FrameType.h"

ValueAxis::ValueAxis(uint16_t _ledCount, AnimationReader *_animationReader) :
    ledCount(_ledCount),
    functionIndices(NULL),
    functionIndicesEntryCount(0),
    animationReader(_animationReader)
{
}

void ValueAxis::initialise(void) {

    valueAxisLowValue = animationReader->readSignedByte();
    Serial.print("value axis low value : ");
    Serial.print(valueAxisLowValue, DEC);
    Serial.println();
    valueAxisHighValue = animationReader->readSignedByte();
    Serial.print("value axis high value : ");
    Serial.print(valueAxisHighValue, DEC);
    Serial.println();
    valueAxisCentreValue = animationReader->readSignedByte();
    Serial.print("value axis zero value : ");
    Serial.print(valueAxisCentreValue, DEC);
    Serial.println();

    allocateFunctionIndices();

    initializeFunctionIndices();

    readFunctionIndices();

}


void ValueAxis::allocateFunctionIndices(void) {
    functionIndicesEntryCount = -valueAxisLowValue + valueAxisHighValue;
    if (
        valueAxisCentreValue != valueAxisLowValue
        &&
        valueAxisCentreValue != valueAxisHighValue
    ) {
        functionIndicesEntryCount++;
    }

    Serial.print("functionIndicesEntryCount: ");
    Serial.print(functionIndicesEntryCount, DEC);
    Serial.println();

    const size_t row_pointers_bytes = ledCount * sizeof *functionIndices;
    const size_t row_elements_bytes = functionIndicesEntryCount * sizeof(uint8_t);
    const size_t memoryToAllocate = row_pointers_bytes + (ledCount * row_elements_bytes);

    Serial.print("memoryToAllocate (rows, row pointer size, columns*row element size, total): ");
    Serial.print(ledCount, DEC);
    Serial.print(", ");
    Serial.print(row_pointers_bytes, DEC);
    Serial.print(", ");
    Serial.print(functionIndicesEntryCount, DEC);
    Serial.print("*");
    Serial.print(row_elements_bytes, DEC);
    Serial.print(", ");
    Serial.print(memoryToAllocate, DEC);
    Serial.println();

#ifdef USE_MULTIPLE_MALLOC_CALLS_FOR_MULTIDIMENSIONAL_ARRAYS

    Serial.print("Allocating function indices...");
    functionIndices = (uint8_t **) malloc(row_pointers_bytes);
    verifyMemoryAllocation((void *)functionIndices);
    Serial.println("OK");

    Serial.print("Allocating function indices rows: ");
    for(size_t i = 0; i < ledCount; i++) {
        functionIndices[i] = (uint8_t *) malloc(row_elements_bytes);
        verifyMemoryAllocation((void *)functionIndices[i]);
        Serial.print("#");
    }
    Serial.println(" OK");
#else
    // FIXME is this really correct?

    functionIndices = (uint8_t **) malloc(memoryToAllocate);

    Serial.print("functionIndices: ");
    Serial.print((uint32_t)functionIndices, HEX);
    Serial.println();

    uint8_t *data = (uint8_t *)functionIndices + sizeof(uint8_t*) * ledCount;
    for(size_t i = 0; i < ledCount; i++) {
        functionIndices[i] = data + i * functionIndicesEntryCount;
        Serial.print("functionIndices[");
        Serial.print(i, DEC);
        Serial.print("]: ");
        Serial.print((uint32_t)functionIndices[i], HEX);
        Serial.println();
    }
#endif
    Serial.println("functionIndices allocated");
    showFreeRam();
}

void ValueAxis::initializeFunctionIndices(void) {
    for (uint16_t i = 0; i < ledCount; i++) {
#ifdef DEBUG_INITIALIZATION_OF_DATA
        Serial.print("Row: ");
        Serial.print(i, DEC);
        Serial.print(" - ");
#endif
        for (uint16_t j = 0; j < functionIndicesEntryCount; j++) {
#if 0
            Serial.print((uint32_t)&functionIndices[i], HEX);
#endif
            functionIndices[i][j] = 0;
#ifdef DEBUG_INITIALIZATION_OF_DATA
            Serial.print("#");
#endif
#if 0
            Serial.print(": ");
            Serial.print(functionIndices[i][j], HEX);
            Serial.println();
#endif
        }
#ifdef DEBUG_INITIALIZATION_OF_DATA
        Serial.println("..OK");
#endif
    }
    Serial.println("functionIndicies initialised");
}


void ValueAxis::readFunctionIndices(void) {

    uint16_t valueAxisOffset;
    
    valueAxisOffset = -valueAxisLowValue;
    Serial.print("valueAxisOffset: ");
    Serial.print(valueAxisOffset, DEC);
    Serial.println();

    for (int8_t valueAxisValue = valueAxisLowValue; valueAxisValue <= valueAxisHighValue;
            valueAxisValue++) {

#if 0
        Serial.print("valueAxisValue: ");
        Serial.print(valueAxisValue, DEC);
        Serial.print(", ledChunkOffset: ");
        Serial.print(animationByteOffset, HEX);
        Serial.println();
#endif

        for (uint16_t ledIndex = 0; ledIndex < ledCount; ledIndex++) {
            uint8_t frameType = animationReader->readUnsignedByte();

            uint16_t valueAxisIndex = valueAxisOffset + valueAxisValue;

            uint8_t functionIndex;

            switch (frameType) {
                case FT_FUNCTION:

                    functionIndex = animationReader->readUnsignedByte();
                    functionIndices[ledIndex][valueAxisIndex] = functionIndex;

#if 0
                    if (functionIndices[ledIndex][valueAxisIndex] != 0) {
                        Serial.print(", function: ");
                        Serial.print(functionIndices[ledIndex][valueAxisIndex], DEC);
                    }
#endif
                break;
                case FT_LINKED:
                    functionIndex = 255;
                    functionIndices[ledIndex][valueAxisIndex] = functionIndex;
                break;
            }
#if 0
            Serial.println();
#endif
        }
    }
}



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

ValueAxis::ValueAxis(uint16_t ledCount, AnimationReader *animationReader) :
    ledCount(ledCount),
    valueAxisLowValue(0),
    valueAxisCentreValue(0),
    valueAxisHighValue(0),
    ledFunctionRanges(NULL),
    animationReader(animationReader)
{
}

ValueAxis::~ValueAxis(void) {
    if (!ledFunctionRanges) {
        return;
    }

    for(uint16_t ledIndex = 0; ledIndex < ledCount; ledIndex++) {
        LedFunctionRanges *ledFunctionRange = ledFunctionRanges[ledIndex];

        if (ledFunctionRange) {
            delete ledFunctionRange;
        }

        ledFunctionRanges[ledIndex] = 0;
    }

#if 1
    delete[] ledFunctionRanges;
#else
    free(ledFunctionRanges);
#endif
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

    allocateFunctionRanges();

    readFunctionRanges();

}

void ValueAxis::allocateFunctionRanges(void) {

    Serial.print("Allocating ledFunctionRanges...");
#if 1
    ledFunctionRanges = new LedFunctionRanges *[ledCount];
#else
    const size_t memoryToAllocate = ledCount * sizeof *ledFunctionRanges;

    ledFunctionRanges = (LedFunctionRanges **) malloc(memoryToAllocate);
#endif

    verifyMemoryAllocation((void *)ledFunctionRanges);
    Serial.println("OK");
}

void ValueAxis::readFunctionRanges(void) {
    uint8_t rangeCount;
    for(uint16_t ledIndex = 0; ledIndex < ledCount; ledIndex++) {
		rangeCount = animationReader->readUnsignedByte();

		LedFunctionRanges *ledFunctionRange = new LedFunctionRanges(ledIndex, rangeCount, animationReader);
		ledFunctionRanges[ledIndex] = ledFunctionRange;
	}

    for(uint16_t ledIndex = 0; ledIndex < ledCount; ledIndex++) {
        LedFunctionRanges *ledFunctionRange = ledFunctionRanges[ledIndex];
        rangeCount = animationReader->readUnsignedByte();
        ledFunctionRange->initialise();
    }
}

uint8_t ValueAxis::retrieveFunctionIndex(uint16_t ledIndex, int8_t valueAxisValue) {
    LedFunctionRanges *ledFunctionRangesForLed = ledFunctionRanges[ledIndex];
    uint8_t ledFunctionIndex = ledFunctionRangesForLed->retrieveFunctionIndex(valueAxisValue);

    return ledFunctionIndex;
}


/*
 * LedFunctionRanges.cpp
 *
 *  Created on: 26 Jan 2014
 *      Author: hydra
 */

#include "WProgram.h"

#include "LedFunctionRanges.h"

LedFunctionRanges::LedFunctionRanges(uint16_t ledIndex, uint8_t rangeCount, AnimationReader *animationReader) :
    ledIndex(ledIndex),
    rangeCount(rangeCount),
    functionRanges(NULL),
    animationReader(animationReader) {
}

LedFunctionRanges::~LedFunctionRanges() {
    if (!functionRanges) {
        return;
    }

    for(uint8_t rangeIndex = 0; rangeIndex < rangeCount; rangeIndex++) {
        FunctionRange *functionRange = functionRanges[rangeIndex];

        if (functionRange) {
            delete functionRange;
        }

        functionRanges[rangeIndex] = NULL;
    }

    delete[] functionRanges;
}

void LedFunctionRanges::initialise(void) {

    allocateRangeIndex();

    for(uint8_t rangeIndex = 0; rangeIndex < rangeCount; rangeIndex++) {

        int8_t low = animationReader->readSignedByte();
        int8_t high = animationReader->readUnsignedByte();
        int8_t anchor = animationReader->readUnsignedByte();
        uint8_t functionRef = animationReader->readUnsignedByte();

        FunctionRange *functionRange = new FunctionRange(low, high, anchor, functionRef);

        functionRanges[rangeIndex] = functionRange;
    }
}

void LedFunctionRanges::allocateRangeIndex(void) {
    functionRanges = new FunctionRange*[rangeCount];

    memset(static_cast<void *>(functionRanges), 0, sizeof(FunctionRange*) * rangeCount);
}


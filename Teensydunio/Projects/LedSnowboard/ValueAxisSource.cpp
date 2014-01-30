/*
 * ValueAxisSource.cpp
 *
 *  Created on: 30 Jan 2014
 *      Author: hydra
 */

#include "WProgram.h"

#include "Config.h"
#include "System.h"

#include "File/FileReader.h"
#include "AnimationReader.h"
#include "LedFunctionRanges.h"

#include "ValueAxis.h"

#include "ValueAxisSource.h"

ValueAxisSource::ValueAxisSource(ValueAxis **valueAxes, uint8_t valueAxisCount) :
    valueAxisCount(valueAxisCount),
    valueAxisPositions(NULL)
{
    size_t memoryToAllocate = valueAxisCount * sizeof(int8_t *);
    Serial.print("memoryToAllocate: ");
    Serial.print(memoryToAllocate, DEC);
    Serial.println();

    valueAxisPositions = (int8_t *)malloc(memoryToAllocate);
    verifyMemoryAllocation((void *)valueAxisPositions);
    showFreeRam();

    for (uint8_t valueAxisIndex = 0; valueAxisIndex < valueAxisCount;
            valueAxisIndex++) {
        ValueAxis *valueAxis = valueAxes[valueAxisIndex];
        valueAxisPositions[valueAxisIndex] = valueAxis->valueAxisCentreValue;
    }
}

ValueAxisSource::~ValueAxisSource() {
    if (valueAxisPositions) {
        free(valueAxisPositions);
        valueAxisPositions = 0;
    }
}


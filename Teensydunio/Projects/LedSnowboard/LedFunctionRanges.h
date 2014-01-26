/*
 * LedFunctionRanges.h
 *
 *  Created on: 26 Jan 2014
 *      Author: hydra
 */

#ifndef LEDFUNCTIONRANGES_H_
#define LEDFUNCTIONRANGES_H_

#include "AnimationReader.h"
#include "FunctionRange.h"

class LedFunctionRanges {
public:
    LedFunctionRanges(uint16_t ledIndex, uint8_t rangeCount, AnimationReader *animationReader);
    virtual ~LedFunctionRanges();

    void initialise(void);

    uint8_t retrieveFunctionIndex(int8_t valueAxisValue);

private:
    uint16_t ledIndex;
    uint8_t rangeCount;

    FunctionRange **functionRanges;

    AnimationReader *animationReader;

    void allocateRangeIndex(void);
};


#endif /* LEDFUNCTIONRANGES_H_ */

/*
 * ValueAxis.h
 *
 *  Created on: 20 Jan 2014
 *      Author: hydra
 */

#ifndef VALUEAXIS_H_
#define VALUEAXIS_H_

#include "AnimationReader.h"
#include "LedFunctionRanges.h"

class ValueAxis {
public:
    ValueAxis(uint16_t ledCount, AnimationReader *animationReader);
    virtual ~ValueAxis(void);

    uint16_t ledCount;

    int8_t valueAxisLowValue;
    int8_t valueAxisCentreValue;
    int8_t valueAxisHighValue;

    LedFunctionRanges **ledFunctionRanges;

    void initialise(void);
    
private:
    void allocateFunctionRanges(void);
    void readFunctionRanges(void);

    AnimationReader *animationReader; 
};

#endif /* VALUEAXIS_H_ */

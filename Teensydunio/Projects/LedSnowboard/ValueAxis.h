/*
 * ValueAxis.h
 *
 *  Created on: 20 Jan 2014
 *      Author: hydra
 */

#ifndef VALUEAXIS_H_
#define VALUEAXIS_H_

#include "AnimationReader.h"

class ValueAxis {
public:
    ValueAxis(uint16_t ledCount, AnimationReader *animationReader);
    virtual ~ValueAxis(void);

    uint16_t ledCount;
    
    uint8_t **functionIndices;
    uint16_t functionIndicesEntryCount;
    
    int8_t valueAxisLowValue;
    int8_t valueAxisHighValue;
    int8_t valueAxisCentreValue;
    
    void initialise(void);
    
private:
    void allocateFunctionIndices(void);
    void initializeFunctionIndices(void);
    void readFunctionIndices(void);
    
    void allocateFunctionRanges(void);
    void readFunctionRanges(void);

    AnimationReader *animationReader; 
    
};

#endif /* VALUEAXIS_H_ */

/*
 * ValueAxis.h
 *
 *  Created on: 20 Jan 2014
 *      Author: hydra
 */

#ifndef VALUEAXIS_H_
#define VALUEAXIS_H_

class ValueAxis {
public:
    ValueAxis();

    int8_t valueAxisLowValue;
    int8_t valueAxisHighValue;
    int8_t valueAxisCentreValue;
    uint8_t **functionIndices;
    uint16_t functionIndicesEntryCount;
};

#endif /* VALUEAXIS_H_ */

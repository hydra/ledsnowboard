/*
 * ValueAxisSource.h
 *
 *  Created on: 30 Jan 2014
 *      Author: hydra
 */

#ifndef VALUEAXISSOURCE_H_
#define VALUEAXISSOURCE_H_

class ValueAxis;

class ValueAxisSource {
public:
    ValueAxisSource(ValueAxis **valueAxes, uint8_t valueAxisCount);
    virtual ~ValueAxisSource();

    inline int8_t retrievePosition(uint8_t valueAxisIndex) {
        return valueAxisPositions[valueAxisIndex];
    }

    inline void applyValueAxisPosition(uint8_t valueAxisIndex, int8_t position) {
        valueAxisPositions[valueAxisIndex] = position;
    }

private:
    uint8_t valueAxisCount;
    int8_t *valueAxisPositions;
};

#endif /* VALUEAXISSOURCE_H_ */

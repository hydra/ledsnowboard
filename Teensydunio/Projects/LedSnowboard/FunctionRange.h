/*
 * FunctionRange.h
 *
 *  Created on: Jan 25, 2014
 *      Author: anne
 */

#ifndef FUNCTIONRANGE_H_
#define FUNCTIONRANGE_H_

class FunctionRange {
public:
    FunctionRange(
            int8_t low,
            int8_t high,
            int8_t anchor,
            uint8_t functionRef);
    virtual ~FunctionRange();

    inline bool appliesTo(int8_t valueAxisValue) {
        return isInRange(valueAxisValue) && valueAxisValue != anchor;
    }

    inline uint8_t getFunctionRef(void) {
        return functionRef;
    }

private:
    int8_t low;
    int8_t high;
    int8_t anchor;
    uint8_t functionRef;

    inline bool isInRange(int8_t valueAxisValue) {
        return valueAxisValue >= low && valueAxisValue <= high;
    }
};

#endif /* FUNCTIONRANGE_H_ */

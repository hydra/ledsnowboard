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

private:
    int8_t low;
    int8_t high;
    int8_t anchor;
    uint8_t functionRef;
};

#endif /* FUNCTIONRANGE_H_ */

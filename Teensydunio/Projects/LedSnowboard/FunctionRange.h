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
	FunctionRange(int8_t lowValue,
					int8_t highValue,
					int8_t zeroValue,
					uint8_t functionNumber);
	virtual ~FunctionRange();

private:
	int8_t lowValue;
	int8_t highValue;
	int8_t zeroValue;
	uint8_t functionNumber;
};

#endif /* FUNCTIONRANGE_H_ */

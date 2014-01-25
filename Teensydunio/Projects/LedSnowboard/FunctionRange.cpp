/*
 * FunctionRange.cpp
 *
 *  Created on: Jan 25, 2014
 *      Author: anne
 */

#include "FunctionRange.h"

FunctionRange::FunctionRange(int8_t lowValue,
		int8_t highValue,
		int8_t zeroValue,
		uint8_t functionNumber) :
		lowValue(lowValue),
		highValue(highValue),
		zeroValue(zeroValue),
		functionNumber(functionNumber) {

}

FunctionRange::~FunctionRange() {
	// TODO Auto-generated destructor stub
}


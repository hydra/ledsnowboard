/*
 * FunctionRange.cpp
 *
 *  Created on: Jan 25, 2014
 *      Author: anne
 */

#include "WProgram.h"

#include "FunctionRange.h"

FunctionRange::FunctionRange(
        int8_t low,
		int8_t high,
		int8_t anchor,
		uint8_t functionRef) :
		low(low),
		high(high),
		anchor(anchor),
		functionRef(functionRef) {

}

FunctionRange::~FunctionRange() {
}

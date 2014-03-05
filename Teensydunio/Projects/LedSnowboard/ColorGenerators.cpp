#include <Arduino.h>

#include "Rgb.h"
#include "RgbLed.h"
#include "Palette.h"
#include "AccelerationData.h"
#include "ScaleMath.h"
#include "SensorDataStore.h"

#include "ColorGenerator.h"

#include "ColorGenerators.h"

extern const Palette *currentPalette;

#define AXIS_X_MIN -4000
#define AXIS_X_MAX 4000

LeftRightTiltColorGenerator::LeftRightTiltColorGenerator(SensorDataStore& sensorDataStore) :
    		sensorDataStore(sensorDataStore), targetColorIndex(UNDEFINED_COLOR_INDEX) {
}

RGB_t &LeftRightTiltColorGenerator::generate(void) {

	uint8_t colorIndex = chooseColorIndex(sensorDataStore.sampledAccelerationData.x, AXIS_X_MIN, AXIS_X_MAX, currentPalette->totalColors);

	//Serial.println(colorIndex, DEC);
	if (colorIndex != targetColorIndex) {
		targetColor = currentPalette->colors[colorIndex];
		targetColorIndex = colorIndex;
		Serial.print("targetColorIndex: ");
		Serial.println(targetColorIndex);
	}

	adjustCurrentColor();

	return currentColor;
}

void LeftRightTiltColorGenerator::reset(void) {
	targetColorIndex = UNDEFINED_COLOR_INDEX;
}

uint8_t LeftRightTiltColorGenerator::chooseColorIndex(int16_t axisValue, int axisMin, int axisMax, uint8_t colorsInPalette) {

	//Serial.println(sensorDataStore.sampledAccelerationData.x, DEC);
	int16_t limitedAxisValue = axisValue;
	if (limitedAxisValue < axisMin) {
		limitedAxisValue = axisMin;
	} else if (limitedAxisValue > axisMax) {
		limitedAxisValue = axisMax;
	}
	return (uint8_t) scaleRange(limitedAxisValue, axisMin, axisMax, 0, colorsInPalette - 1);
}

uint8_t LeftRightTiltColorGenerator::adjustColorComponent(uint8_t current, uint8_t target) {

	if (current == target) {
		return current;
	}

	int16_t adjusted = 0;

	if (current > target) {
		adjusted = current - COLOR_ADJUST_STEP;
		if (adjusted < target) {
			adjusted = target;
		}
	}
	if (current < target) {
		adjusted = current + COLOR_ADJUST_STEP;
		if (adjusted > target) {
			adjusted = target;
		}
	}

	if (adjusted < 0) {
		return 0;
	}
	if (adjusted > 255) {
		return 255;
	}

	return adjusted;
}

void LeftRightTiltColorGenerator::adjustCurrentColor(void) {
	currentColor.r = adjustColorComponent(currentColor.r, targetColor.r);
	currentColor.g = adjustColorComponent(currentColor.g, targetColor.g);
	currentColor.b = adjustColorComponent(currentColor.b, targetColor.b);
}

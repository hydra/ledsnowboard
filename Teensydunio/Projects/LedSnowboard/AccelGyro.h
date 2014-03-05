/*
 * AccelGyro.h
 *
 *  Created on: 14 Jan 2014
 *      Author: hydra
 */

#ifndef ACCELGYRO_H_
#define ACCELGYRO_H_

#include "I2Cdev.h"
#include "MPU6050.h"
#include "StatusLed.h"
#include "SensorDataStore.h"
#include "Sampler.h"

class NumberRange {
public:
    int16_t min, zero, max;

    void updateLimits(int16_t value);
    void reset(int16_t value);
};

class AccelGyro {
public:
    AccelGyro(StatusLed& statusLed, SensorDataStore& sensorDataStore, Sampler& sampler);

    void configure();

    MPU6050 impl;

    inline NumberRange *getXRange(void) { return &xRange; }
    inline NumberRange *getYRange(void) { return &yRange; }

    void refresh();

    AccelerationData *getLatestSample(void);

private:
    StatusLed& statusLed;
    SensorDataStore& sensorDataStore;
    Sampler& sampler;

    NumberRange xRange;
    NumberRange yRange;

    bool isZeroAxInitialized;
    bool isZeroAyInitialized;

    void updateXRange(int16_t value);
    void updateYRange(int16_t value);
};

#endif /* ACCELGYRO_H_ */

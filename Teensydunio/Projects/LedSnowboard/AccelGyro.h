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

class AccelGyro {
public:
    AccelGyro(StatusLed statusLed, SensorDataStore sensorDataStor, Sampler sampler);

    void configure();

    MPU6050 impl;

    signed int getNormalisedXValue();
    signed int getNormalisedYValue();

    void refresh();

private:
    StatusLed statusLed;
    SensorDataStore sensorDataStore;
    Sampler sampler;

    int8_t x;
    int16_t minAx, maxAx, zeroAx;

    int8_t y;
    int16_t minAy, maxAy, zeroAy;

    bool isZeroAxInitialized;
    bool isZeroAyInitialized;


    void normalizeAx(void);
    void normalizeAy(void);
};

#endif /* ACCELGYRO_H_ */

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

class AccelGyro {
public:
    AccelGyro(StatusLed statusLed);

    void configure();

    MPU6050 impl;

    signed int getNormalisedAccelerometerXValue();

private:
    StatusLed statusLed;

    int16_t ax, ay, az;
    int16_t gx, gy, gz;

    int16_t minAx, maxAx, zeroAx;

    bool isZeroAxInitialized;

    void refresh();

};

#endif /* ACCELGYRO_H_ */

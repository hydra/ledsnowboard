
/*
 * AccelGyro.cpp
 *
 *  Created on: 14 Jan 2014
 *      Author: hydra
 */
#include "Config.h"

#include "AccelGyro.h"

AccelGyro::AccelGyro(StatusLed& statusLed, SensorDataStore& sensorDataStore, Sampler& sampler) :
        statusLed(statusLed),
        sensorDataStore(sensorDataStore),
        sampler(sampler),
        isZeroAxInitialized(false),
        isZeroAyInitialized(false) {
    xRange.reset(0);
    yRange.reset(0);
}

void AccelGyro::configure() {
    statusLed.enable();

    // initialize device
    Serial.println("Initializing I2C devices...");
    while (true) {
        impl.initialize();

        // verify connection
        Serial.println("Testing device connections...");
        if (impl.testConnection()) {
            Serial.println("MPU6050 connection successful");
            break;
        } else {
            Serial.println("MPU6050 connection failed");
            int togglesLeft = 5;
            while (togglesLeft--) {
                statusLed.toggle();
                delay(500);
            }
        }
    }

    statusLed.disable();
    refresh();
}

void AccelGyro::updateXRange(int16_t value) {
    xRange.updateLimits(value);
}

void AccelGyro::updateYRange(int16_t value) {
    yRange.updateLimits(value);
}

void NumberRange::reset(int16_t value) {
    zero = value;
    min = value;
    max = value;
}

void NumberRange::updateLimits(int16_t value) {
    bool changed = false;

    if (value < min && value < zero - 200) {
        if (abs(value) < (abs(min) * 10L)) { // avoid crazy readings
            min = value;
            changed = true;
        }
    }

    if (value > max && value > zero + 200) {
        if (abs(value) < (abs(max) * 10L)) { // avoid crazy readings
            max = value;
            changed = true;
        }
    }

    if (!changed) {
        return;
    }

#ifdef DEBUG_ACCEL_MIN_MAX
    Serial.print("min/max (min,max): (");
    Serial.print(min, DEC);
    Serial.print(",");
    Serial.print(max, DEC);
    Serial.println(")");
#endif
}

AccelerationData *AccelGyro::getLatestSample(void) {
    return &sensorDataStore.sampledAccelerationData;
}

void AccelGyro::refresh() {
    impl.getAcceleration(&sensorDataStore.rawAccelerationData.x, &sensorDataStore.rawAccelerationData.y, &sensorDataStore.rawAccelerationData.z);
    
    sampler.addAccelerationData(sensorDataStore.rawAccelerationData);
    if (!sampler.isReady()) {
        return;
    }

    sensorDataStore.sampledAccelerationData = sampler.getSample();
    sensorDataStore.sampledAccelerationData.scale(2); // ensure that later, when calculating limits, that int16_t is not overflowed.
    sensorDataStore.isSampledAccelerationDataValid = true;

    sampler.prepareForUpdate();

    const int16_t ax = sensorDataStore.sampledAccelerationData.x;
    const int16_t ay = sensorDataStore.sampledAccelerationData.y;

    if (!isZeroAxInitialized) {
#ifdef DEBUG_ACCEL_MIN_MAX
        Serial.print("x zero :");
        Serial.println(ax, DEC);
#endif
        xRange.reset(ax);
        isZeroAxInitialized = true;
    }
    if (!isZeroAyInitialized) {

#ifdef DEBUG_ACCEL_MIN_MAX
        Serial.print("y zero :");
        Serial.println(ay, DEC);
#endif
        yRange.reset(ay);
        isZeroAyInitialized = true;
    }
    updateXRange(ax);
    updateYRange(ay);

#ifdef DEBUG_ALL_ACCEL_DATA
    Serial.print("accel (rawX,rawY,rawZ) (minX,zeroX,maxX) (minY,zeroY,maxY): (");
    Serial.print(sensorDataStore.sampledAccelerationData.x, DEC);
    Serial.print(",");
    Serial.print(sensorDataStore.sampledAccelerationData.y, DEC);
    Serial.print(",");
    Serial.print(sensorDataStore.sampledAccelerationData.z, DEC);
    Serial.print(") (");
    Serial.print(xRange.min, DEC);
    Serial.print(",");
    Serial.print(xRange.zero, DEC);
    Serial.print(",");
    Serial.print(xRange.max, DEC);
    Serial.print(") (");
    Serial.print(yRange.min, DEC);
    Serial.print(",");
    Serial.print(yRange.zero, DEC);
    Serial.print(",");
    Serial.print(yRange.max, DEC);
    Serial.println(")");
#endif
}

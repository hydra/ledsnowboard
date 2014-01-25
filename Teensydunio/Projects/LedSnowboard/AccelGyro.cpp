
/*
 * AccelGyro.cpp
 *
 *  Created on: 14 Jan 2014
 *      Author: hydra
 */
#include "Config.h"

#include "AccelGyro.h"

AccelGyro::AccelGyro(StatusLed statusLed, SensorDataStore sensorDataStore, Sampler sampler) :
        statusLed(statusLed),
        sensorDataStore(sensorDataStore),
        sampler(sampler),
        x(0),
        minAx(0),
        maxAx(0),
        zeroAx(0),
        y(0),
        minAy(0),
        maxAy(0),
        zeroAy(0),
        isZeroAxInitialized(false) {
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

int AccelGyro::getNormalisedYValue() {
    return y;
}

int AccelGyro::getNormalisedXValue() {
    return x;
}


static int8_t normalizeValue(int16_t reading, int16_t minReading, int16_t maxReading, int16_t zeroReading) {

    // FIXME clearly this is test code because it does not take into account the min/max/zero values
    int16_t adjustedReading = reading - zeroReading;

    for (int8_t r = -50; r <= 50; r++) {
        int16_t rangeMin = abs(r) * 200;
        int16_t rangeMax = (abs(r)+1) * 200;
        if ((reading < zeroReading && r < -1) || (reading > zeroReading && r > 1)) {
            if ((r == -50 || r == 50) && abs(adjustedReading) > rangeMax) {
                return r;
            }
            if (abs(adjustedReading) > rangeMin && abs(adjustedReading) < rangeMax) {
                return r;
            }
        }
    }
    return 0;
}

void AccelGyro::normalizeAx(void) {
    int16_t ax = sensorDataStore.sampledAccelerationData.x;
    if (ax < minAx && ax < zeroAx - 200) {
        if (abs(ax) < (abs(minAx) * 10L)) { // avoid crazy readings
            minAx = ax;
            Serial.print("new x min :");
            Serial.print(ax, DEC);
            Serial.println();
        }
    }
    if (ax > maxAx && ax > zeroAx + 200) {
        if (abs(ax) < (abs(maxAx) * 10L)) { // avoid crazy readings
            maxAx = ax;
            Serial.print("new x max :");
            Serial.print(ax, DEC);
            Serial.println();
        }
    }

    x = normalizeValue(ax, minAx, maxAx, zeroAx);
}

void AccelGyro::normalizeAy(void) {

    int16_t ay = sensorDataStore.sampledAccelerationData.y;
    if (ay < minAy && ay < zeroAy - 200) {
        if (abs(ay) < (abs(minAy) * 10L)) { // avoid crazy readings
            minAy = ay;
            Serial.print("new y min:");
            Serial.print(ay, DEC);
            Serial.println();
        }
    }
    if (ay > maxAy && ay > zeroAy + 200) {
        if (abs(ay) < (abs(maxAy) * 10L)) { // avoid crazy readings
            maxAy = ay;
            Serial.print("new y max :");
            Serial.print(ay, DEC);
            Serial.println();
        }
    }

    y = normalizeValue(ay, minAy, maxAy, zeroAy);
}

void AccelGyro::refresh() {
    impl.getAcceleration(&sensorDataStore.rawAccelerationData.x, &sensorDataStore.rawAccelerationData.y, &sensorDataStore.rawAccelerationData.z);
    
    sampler.addAccelerationData(sensorDataStore.rawAccelerationData);
    if (!sampler.isReady()) {
        return;
    }
    
    sensorDataStore.sampledAccelerationData = sampler.getSample();

#ifdef LOG_SAMPLE
    Serial.print(", x: ");
    Serial.print(sensorDataStore.sampledAccelerationData.x);
    Serial.print("\t, y: ");
    Serial.print(sensorDataStore.sampledAccelerationData.y);
    Serial.print("\t, z: ");
    Serial.println(sensorDataStore.sampledAccelerationData.z);
#endif

    sampler.prepareForUpdate();
    

    if (!isZeroAxInitialized) {

        int16_t ax = sensorDataStore.sampledAccelerationData.x;
        Serial.print("new x zero :");
        Serial.print(ax, DEC);
        Serial.println();
        zeroAx = ax;
        minAx = ax;
        maxAx = ax;

        isZeroAxInitialized = true;
    }
    if (!isZeroAyInitialized) {

        int16_t ay = sensorDataStore.sampledAccelerationData.y;
        Serial.print("new y zero :");
        Serial.print(ay, DEC);
        Serial.println();
        zeroAy = ay;
        minAy = ay;
        maxAy = ay;

        isZeroAyInitialized = true;
    }
    normalizeAx();
    normalizeAy();

#ifdef DEBUG_ALL_ACCEL_DATA
    Serial.print("accel (rawX,rawY,rawZ) (minX,zeroX,maxX) (minY,zeroY,maxY) (x,y): (");
    Serial.print(sensorDataStore.sampledAccelerationData.x, DEC);
    Serial.print(",");
    Serial.print(sensorDataStore.sampledAccelerationData.y, DEC);
    Serial.print(",");
    Serial.print(sensorDataStore.sampledAccelerationData.z, DEC);
    Serial.print(") (");
    Serial.print(minAx, DEC);
    Serial.print(",");
    Serial.print(zeroAx, DEC);
    Serial.print(",");
    Serial.print(maxAx, DEC);
    Serial.print(") (");
    Serial.print(minAy, DEC);
    Serial.print(",");
    Serial.print(zeroAy, DEC);
    Serial.print(",");
    Serial.print(maxAy, DEC);
    Serial.print(") (");
    Serial.print(x, DEC);
    Serial.print(",");
    Serial.print(y, DEC);
    Serial.println(")");
#endif

#ifdef DEBUG_NORMALIZED_XY_ACCEL_DATA
    Serial.print("accel (x,y): (");
    Serial.print(x, DEC);
    Serial.print(",");
    Serial.print(y, DEC);
    Serial.println(")");
#endif

}

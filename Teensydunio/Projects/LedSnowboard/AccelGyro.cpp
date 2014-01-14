
/*
 * AccelGyro.cpp
 *
 *  Created on: 14 Jan 2014
 *      Author: hydra
 */
#include "Config.h"

#include "AccelGyro.h"

AccelGyro::AccelGyro(StatusLed statusLed) :
        statusLed(statusLed),
        minAx(0),
        maxAx(0),
        zeroAx(0),
        isZeroAxInitialized(false) {
}

void AccelGyro::configure() {
    statusLed.disable();

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

    statusLed.enable();
}

int AccelGyro::getNormalisedAccelerometerXValue() {

    refresh();

    if (ax < minAx && ax < zeroAx - 200) {
        minAx = ax;
        Serial.print("new min :");
        Serial.print(ax, DEC);
        Serial.print("\n");
    }
    if (ax > maxAx && ax > zeroAx + 200) {
        maxAx = ax;
        Serial.print("new max :");
        Serial.print(ax, DEC);
        Serial.print("\n");
    }

    if (ax > zeroAx + 200) {
        return 1;
    } else if (ax < zeroAx - 200) {
        return -1;
    } else {
        return 0;
    }
}

void AccelGyro::refresh() {
    impl.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

    if (!isZeroAxInitialized) {
        Serial.print("new zero :");
        Serial.print(ax, DEC);
        Serial.print("\n");
        zeroAx = ax;

        isZeroAxInitialized = true;
    }

}

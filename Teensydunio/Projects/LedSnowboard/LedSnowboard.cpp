
#include "WProgram.h"

#include <OctoWS2811.h>
#include "Wire.h"

#include "Config.h"
#include "Time.h"

#include "StatusLed.h"
#include "AccelGyro.h"
#include "Animation.h"

StatusLed statusLed(LED_PIN);
AccelGyro accelGyro(statusLed);


// CRUFT BELOW HERE

#define LEDS_PER_STRIP 2
#define MEMORY_NEEDED_FOR_EACH_LED 6

DMAMEM int displayMemory[LEDS_PER_STRIP * MEMORY_NEEDED_FOR_EACH_LED];
int drawingMemory[LEDS_PER_STRIP * MEMORY_NEEDED_FOR_EACH_LED];

const int config = WS2811_GRB | WS2811_800kHz;

extern int timeAxisNum;

OctoWS2811 leds( LEDS_PER_STRIP, displayMemory, drawingMemory, config);

void setup() {
     pinMode(LED_PIN, OUTPUT);
      
    #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
      Wire.begin();
    #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
      Fastwire::setup(400, true);
    #endif
      
    Serial.begin(115200);
    // Delay required so the developer has time to connect a serial port monitor to the device.
    delayMicroseconds(MICROSECONDS_IN_A_SECOND * 5);

    accelGyro.configure();

    // delay so that at least some serial output can be seen after accelerometer is configured.
    delayMicroseconds(MICROSECONDS_IN_A_SECOND * 1);

    readAnimationDetails();

    leds.begin();

    Serial.print("FINISHED SETUP");
    Serial.print("\n");
}

void loop() {
    Serial.print("Loop\n");
    animate();

    // if (readByteUnsignedChar(&iCounter) != TERMINATING_BYTE) {
    //throw new InvalidAnimationException("No terminating byte");
    // }
}

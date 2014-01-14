
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

DMAMEM int displayMemory[LEDS_PER_STRIP * 6];
int drawingMemory[LEDS_PER_STRIP * 6];

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
    delayMicroseconds(MICROSECONDS_IN_A_SECOND * 5);

    accelGyro.configure();
    
    readAnimationDetails();

    leds.begin();

    Serial.print("FINISHED SETUP");
    Serial.print("\n");
}

void loop() {
    Serial.print("Loop\n");
    readAxisData(timeAxisNum);

    // if (readByteUnsignedChar(&iCounter) != TERMINATING_BYTE) {
    //throw new InvalidAnimationException("No terminating byte");
    // }
}

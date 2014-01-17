
#include "WProgram.h"

#include <OctoWS2811.h>
#include "Wire.h"

#include "Config.h"
#include "Time.h"

#include "StatusLed.h"
#include "AccelGyro.h"
#include "Animator.h"

#include "Scheduling/ScheduledAction.h"

ScheduledAction statusLedAction;
ScheduledAction animationFrameAdvanceAction;
StatusLed statusLed(LED_PIN);
AccelGyro accelGyro(statusLed);
Animator animator;

ScheduledAction sdCardStatusAction;
ScheduledAction serialStatusAction;


#include <SdFat.h>
#include <SdFatUtil.h>
/*
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
*/
#define SD_CD_PIN 9 // Card detect for SD card
#define SD_CS 10 // Chip select for SD card

bool cardPresence = false;
SdFat sd;

#define LEDS_PER_STRIP 2
#define MEMORY_NEEDED_FOR_EACH_LED 6

DMAMEM int displayMemory[LEDS_PER_STRIP * MEMORY_NEEDED_FOR_EACH_LED];
int drawingMemory[LEDS_PER_STRIP * MEMORY_NEEDED_FOR_EACH_LED];

const int config = WS2811_GRB | WS2811_800kHz;

OctoWS2811 leds( LEDS_PER_STRIP, displayMemory, drawingMemory, config);


void checkCardPresence(void) {
  bool newCardPresence = digitalRead(SD_CD_PIN);
  if (newCardPresence != cardPresence) {
    cardPresence = newCardPresence;
    if (cardPresence == HIGH) {
      Serial.println("SD card removed");
    } else {
      Serial.println("SD card inserted");
    }
  }  
}

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

    // configure SdCard hardware
    pinMode(SD_CD_PIN, INPUT);


    while(true) {
      checkCardPresence();
    
      Serial.print("Initializing SD card...");

      if (!sd.begin(SD_CS, SPI_FULL_SPEED)) {
        Serial.println("initialization failed!");
        sd.initErrorHalt();
        delay(500);
        continue;
      }
      Serial.println("initialization done.");    
      break;
    }
    

    Serial.println("Volume is FAT");
    Serial.println(sd.vol()->fatType(), DEC);
    Serial.println();

    // list file in root with date and size
    Serial.println("Files found in root:");
    sd.ls(LS_DATE | LS_SIZE);
    Serial.println();
    
    // Recursive list of all directories
    Serial.println("Files found in all dirs:");
    sd.ls(LS_R);
    
    Serial.println();
    
    sdCardStatusAction.setDelayMillis(1000L);
    sdCardStatusAction.reset();

    
    Serial.println("Done");
    
    
    // setup leds and animation 
    animator.readAnimationDetails();

    leds.begin();

    statusLedAction.setDelayMillis(500L);
    statusLedAction.reset();

    serialStatusAction.setDelayMillis(1000L);
    serialStatusAction.reset();

    animationFrameAdvanceAction.setDelayMillis(animator.iTimeAxisSpeed);
    animationFrameAdvanceAction.reset();

    Serial.print("FINISHED SETUP");
    Serial.print("\n");
}

void updateCpuActivityLed(void) {
  if (!statusLedAction.isActionDue()) {
    return;
  }
  
  statusLed.toggle();
}

void updateAnimation() {
    if (!animationFrameAdvanceAction.isActionDue()) {
        return;
    }

    animator.renderNextFrame();
    leds.show();
}

void checkSdCardStatus() {
    if (!sdCardStatusAction.isActionDue()) {
        return;
    }
    checkCardPresence();
}

void updateSerialStatus() {
    if (!serialStatusAction.isActionDue()) {
        return;
    }
    Serial.print(".");
}

void loop() {
    updateCpuActivityLed();
    updateSerialStatus();
    checkSdCardStatus();

    updateAnimation();
}



#include "WProgram.h"

#include <OctoWS2811.h>
#include "Wire.h"

#include <SdFat.h>
#include <SdFatUtil.h>

#include "Config.h"
#include "System.h"
#include "Time.h"

#include "StatusLed.h"
#include "AccelGyro.h"
#include "Animator.h"
#include "Sampler.h"
#include "SensorDataStore.h"

#include "Input.h"
#include "ArduinoDigitalInput.h"
#include "DebouncedInput.h"

#include "Scheduling/ScheduledAction.h"
#include "File/FileReader.h"
#include "File/SdCardFileReader.h"

ScheduledAction statusLedAction;
ScheduledAction animationFrameAdvanceAction;

StatusLed earlyStartupStatusAndSdCardPresenceLed(TEENSY_LED_PIN);
StatusLed cpuStatusLed(CPU_STATUS_LED_PIN);

SensorDataStore sensorDataStore;
Sampler sampler;
AccelGyro accelGyro(earlyStartupStatusAndSdCardPresenceLed, sensorDataStore, sampler);

SdCardFileReader fileReader;
Animator animator;

ScheduledAction sdCardStatusAction;
ScheduledAction serialStatusAction;
ScheduledAction gyroRefreshAction;

bool hasSdCard = false;
bool previouslyHadSdCard = true;

SdFat sd;
SdFile myFile;
SdFile animationFile;

#define LEDS_PER_STRIP 30
#define MEMORY_NEEDED_FOR_EACH_LED 6

const int ledConfig = WS2811_GRB | WS2811_800kHz;

DMAMEM int displayMemory[LEDS_PER_STRIP * MEMORY_NEEDED_FOR_EACH_LED];
#ifdef USE_DRAWING_MEMORY_FOR_LEDS
int drawingMemory[LEDS_PER_STRIP * MEMORY_NEEDED_FOR_EACH_LED];
OctoWS2811 leds( LEDS_PER_STRIP, displayMemory, drawingMemory, ledConfig);
#else
OctoWS2811 leds( LEDS_PER_STRIP, displayMemory, NULL, ledConfig);
#endif

ArduinoDigitalInput backInput;
ArduinoDigitalInput upInput;
ArduinoDigitalInput downInput;
ArduinoDigitalInput selectInput;

DebouncedInput backButton;
DebouncedInput upButton;
DebouncedInput downButton;
DebouncedInput selectButton;

void updateSdCardPresence(void) {
  previouslyHadSdCard = hasSdCard;
  hasSdCard = digitalRead(SD_CD_PIN); // HIGH == not-present, LOW == present
  hasSdCard = !hasSdCard;
}

bool hasSdCardBeenInserted() {
    return (hasSdCard && !previouslyHadSdCard);
}

bool hasSdCardBeenRemoved() {
    return (!hasSdCard && previouslyHadSdCard);
}

void showSdCardInsertionOrRemovalMessage() {
    if (hasSdCardBeenRemoved()) {
        Serial.println("SD card removed");
    }

    if (hasSdCardBeenInserted()) {
        Serial.println("SD card inserted");
    }
}

void setup() {
    earlyStartupStatusAndSdCardPresenceLed.configure();
    cpuStatusLed.configure();
      
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

    leds.begin();

    sdCardStatusAction.setDelayMillis(1000L);
    sdCardStatusAction.reset();

    statusLedAction.setDelayMillis(500L);
    statusLedAction.reset();

    serialStatusAction.setDelayMillis(1000L);
    serialStatusAction.reset();
    
    gyroRefreshAction.setDelayMillis(1000L / SAMPLE_FREQUENCY_HZ);
    gyroRefreshAction.reset();

    showFreeRam();

    // turn light on until SD card insertion disables it.
    earlyStartupStatusAndSdCardPresenceLed.enable();


    backInput.configure(BACK_BUTTON_PIN);
    backButton.setInput(&backInput);
    upInput.configure(UP_BUTTON_PIN);
    upButton.setInput(&upInput);
    downInput.configure(DOWN_BUTTON_PIN);
    downButton.setInput(&downInput);
    selectInput.configure(SELECT_BUTTON_PIN);
    selectButton.setInput(&selectInput);

#ifdef DEBUG_BUTTON_TEST
    Serial.print("Press and release BACK, UP, DOWN then SELECT to begin");

    bool backPressed = false;
    bool upPressed = false;
    bool downPressed = false;
    bool selectPressed = false;

    while (!(backPressed && upPressed && downPressed && selectPressed)) {
        while(backButton.getValue()) { backPressed = true; Serial.println("B"); };
        while(upButton.getValue()) { upPressed = true; Serial.println("U");};
        while(downButton.getValue()) { downPressed = true; Serial.println("D");};
        while(selectButton.getValue()) { selectPressed = true; Serial.println("S");};
    }
#endif


    Serial.print("FINISHED SETUP");
    Serial.print("\n");
}

#if SHOW_SD_CARD_CONTENTS_ON_INSERTION
void showSdCardContents(void) {
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
}
#endif

void onSdCardInserted() {
    earlyStartupStatusAndSdCardPresenceLed.configure();
    earlyStartupStatusAndSdCardPresenceLed.disable();

    bool sdCardInitialised = false;
    uint8_t attemptsRemaining = 5;
    while(attemptsRemaining) {
        Serial.print("Reading SD card...");
        sdCardInitialised = sd.begin(SD_CS, SPI_FULL_SPEED);
        if (sdCardInitialised) {
            break;
        }
        attemptsRemaining--;
        Serial.print("failed, attemptsRemaining: ");
        Serial.print(attemptsRemaining, DEC);
        Serial.println();

        sd.initErrorPrint();

        if (attemptsRemaining) {
            delay(500);
            Serial.println("Retrying SD card");
        }
    }
    if (!sdCardInitialised) {
        return;
    }

    Serial.println("OK");
    showFreeRam();


#if SHOW_SD_CARD_CONTENTS_ON_INSERTION
    showSdCardContents();
#endif

    Serial.print("Opening TEST3.ANI...");

    if (!animationFile.open("TEST3.ANI", O_RDONLY)) {
        sd.errorPrint("opening TEST3.ANI for read failed");
        return;
    }
    Serial.println("OK");
    showFreeRam();


    fileReader.setSdFile(&animationFile);
    // setup leds and animation
    animator.readAnimationDetails(&fileReader);
    showFreeRam();

#ifdef OVERRIDE_ANIMATION_FREQUENCY
    animationFrameAdvanceAction.setDelayMillis(1);
#else
    animationFrameAdvanceAction.setDelayMillis(animator.timeAxisFrequencyMillis);
#endif
    animationFrameAdvanceAction.reset();
}

void onSdCardRemoved(void) {
    animationFile.close();
    animator.reset();
    earlyStartupStatusAndSdCardPresenceLed.configure();
    earlyStartupStatusAndSdCardPresenceLed.enable();
}

void updateCpuActivityLed(void) {
  if (!statusLedAction.isActionDue()) {
    return;
  }
  
  cpuStatusLed.toggle();
}

void updateAnimation() {
    if (!animator.haveAnimation()) {
        return;
    }

    if (!animationFrameAdvanceAction.isActionDue()) {
        return;
    }

    /*
    Serial.print("lateBy: ");
    Serial.print(animationFrameAdvanceAction.getLateBy(), DEC);
    Serial.println();
    */

    animator.renderNextFrame();
    leds.show();
}

void checkSdCardStatus() {
    if (!sdCardStatusAction.isActionDue()) {
        return;
    }

    updateSdCardPresence();
    showSdCardInsertionOrRemovalMessage();

    if (hasSdCardBeenInserted()) {
        onSdCardInserted();
        return;
    }

    if (hasSdCardBeenRemoved()) {
        onSdCardRemoved();
    }

}

void updateSerialStatus() {
    if (!serialStatusAction.isActionDue()) {
        return;
    }
    Serial.print(".");
}


void processGyro(void) {
  if (!gyroRefreshAction.isActionDue()) {
    return;
  }

  accelGyro.refresh();
}

void loop() {
    updateCpuActivityLed();
    updateSerialStatus();
    processGyro();
    checkSdCardStatus();

    updateAnimation();
}


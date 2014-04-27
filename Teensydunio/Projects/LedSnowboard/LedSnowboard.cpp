
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

#include "AnimationScheduler.h"

#include "Menu.h"

#include "Rgb.h"
#include "RgbLed.h"
#include "ColorGenerator.h"
#include "ColorGenerators.h"
#include "HsvRgbConverter.h"
#include "Palette.h"
#include "Animation.h"
#include "Animations.h"

#include "LedGrid.h"

LedGrid ledGrid(GRID_WIDTH, GRID_HEIGHT, ledGridToIndexMapping);

ScheduledAction statusLedAction;
ScheduledAction animationFrameAdvanceAction;

ScheduledAction buttonProcessAction;

StatusLed earlyStartupStatusAndSdCardPresenceLed(TEENSY_LED_PIN);
StatusLed cpuStatusLed(CPU_STATUS_LED_PIN);

SensorDataStore sensorDataStore;
Sampler sampler;
AccelGyro accelGyro(earlyStartupStatusAndSdCardPresenceLed, sensorDataStore, sampler);

SdCardFileReader fileReader;
Animator animator;

RGB_t palette1Colors[] = { { 255, 0, 0 }, { 255, 0, 255 }, { 255, 255, 0 }, { 255, 255, 255 }, { 0, 255, 255 }, { 0, 255, 0 },
    { 0, 0, 255 } };
RGB_t palette2Colors[] = { { 255, 0, 0 }, { 255, 0, 0 }, { 0, 0, 0 }, { 255, 255, 255 }, { 0, 0, 0 }, { 0, 0, 255 }, { 0, 0, 255 } };
RGB_t firePaletteColors[] = { { 255, 0, 0 }, {254, 108, 1 }, { 254, 179, 1 }, { 255, 255, 0 } };
RGB_t lemmingsPaletteColors[] = { { 64, 64, 192 }, { 255, 255, 255 }, {32, 176, 32 } };
/*
RGB_t gnuAlteredGeneticsColors[] = {
    { 178, 176, 101 }, { 138, 127, 120 }, { 119, 104, 64 }, { 77, 67, 69 }, { 236, 231, 217 }
};
*/

Palette const palettes[] = {
    { COLORS_IN_PALETTE(palette1Colors), palette1Colors },
    { COLORS_IN_PALETTE(palette2Colors), palette2Colors },
    { COLORS_IN_PALETTE(firePaletteColors), firePaletteColors },
    { COLORS_IN_PALETTE(lemmingsPaletteColors), lemmingsPaletteColors }
    //{ COLORS_IN_PALETTE(gnuAlteredGeneticsColors), gnuAlteredGeneticsColors }
};

#define TOTAL_PALETTES (sizeof(palettes) / sizeof(Palette))

uint8_t paletteIndex = 0;
const Palette *currentPalette = &palettes[paletteIndex];


LeftRightTiltColorGenerator leftRightTiltColorGenerator(sensorDataStore);

SolidAnimation solidAnimation(leftRightTiltColorGenerator);
FlashAnimation flashAnimation(leftRightTiltColorGenerator);
LineTopToBottomAnimation lineTopToBottomAnimation(leftRightTiltColorGenerator, GRID_HEIGHT);
ThickLineTopToBottomAnimation thickLineTopToBottomAnimation(leftRightTiltColorGenerator, GRID_HEIGHT);
SawtoothFadeAnimation sawtoothFadeAnimation(leftRightTiltColorGenerator);
UpDownFadeAnimation upDownFadeAnimation(leftRightTiltColorGenerator);
CarLightsAnimation carLightsAnimation(sensorDataStore, GRID_WIDTH, GRID_HEIGHT);
MovingRainbowAnimation movingRainbowAnimation(sensorDataStore, GRID_HEIGHT);

#define TOTAL_ANIMATIONS 8
Animation* const animations[] = {
    &movingRainbowAnimation,
    &carLightsAnimation,
    &thickLineTopToBottomAnimation,
    &lineTopToBottomAnimation,
    &upDownFadeAnimation,
    &sawtoothFadeAnimation,
    &flashAnimation,
    &solidAnimation
};

unsigned int animationIndex = 0;
Animation* currentAnimation;
unsigned long usDelay;
unsigned int frameCounter = 0;

ScheduledAction sdCardStatusAction;
ScheduledAction serialStatusAction;
ScheduledAction gyroRefreshAction;

bool isSdCardPresent = false;
bool previouslyHadSdCard = true;
bool isSdCardOpen = false;

SdFat sd;
SdFile myFile;
SdFile animationFile;

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

class ChooseAnimationMenu;
class MainMenu;

AnimationScheduler animationScheduler(&animationFrameAdvanceAction, &animator);

ChooseAnimationMenu chooseAnimationMenu;
FrequencyMenu frequencyMenu(&animationScheduler, &animator);
Menu *subMenus[] = {&chooseAnimationMenu, &frequencyMenu};
MainMenu mainMenu(subMenus, 2);
MenuStack menuStack(&mainMenu, backButton, selectButton, upButton, downButton);


SdBaseFile* root;
char fileName[13];

void updateSdCardPresence(void) {
  previouslyHadSdCard = isSdCardPresent;
  isSdCardPresent = digitalRead(SD_CD_PIN); // HIGH == not-present, LOW == present
  isSdCardPresent = !isSdCardPresent;
}

bool hasSdCardBeenInserted() {
    return (isSdCardPresent && !previouslyHadSdCard);
}

bool hasSdCardBeenRemoved() {
    return (!isSdCardPresent && previouslyHadSdCard);
}

void showSdCardInsertionOrRemovalMessage() {
    if (hasSdCardBeenRemoved()) {
        Serial.println("SD card removed");
    }

    if (hasSdCardBeenInserted()) {
        Serial.println("SD card inserted");
    }
}

void openSdCard(void) {
    isSdCardOpen = false;
    uint8_t attemptsRemaining = 5;
    while(attemptsRemaining) {
        Serial.print("Reading SD card...");
        isSdCardOpen = sd.begin(SD_CS, SPI_FULL_SPEED);
        if (isSdCardOpen) {
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
    if (!isSdCardOpen) {
        return;
    }

    Serial.println("OK");
    showFreeRam();

#if SHOW_SD_CARD_CONTENTS_ON_INSERTION
    showSdCardContents();
#endif

    root = sd.vwd();
}

void resetSdCard(void) {
    // nothing to cleanup

    openSdCard();
}


void updateAnimationDelay(void) {
	usDelay = (1000L * 100) + currentAnimation->getFrameDelayMicros();
	Serial.print("usDelay: ");
	Serial.println(usDelay, DEC);

	animationFrameAdvanceAction.setDelayMicros(usDelay);
	animationFrameAdvanceAction.reset();
}

void resetAnimation(void) {
	currentAnimation = animations[animationIndex];
	currentAnimation->reset();

	Serial.println("Animation reset");

	updateAnimationDelay();
}

void updateSimpleAnimation(void) {

	if (!animationFrameAdvanceAction.isActionDue()) {
		return;
	}

	currentAnimation->frameBegin();

	RGB_t rgb;
	RgbLed rgbLedBuffer;
	rgbLedBuffer.rgb = &rgb;

	for (uint8_t y = 0; y < GRID_HEIGHT; y++) {
#ifdef DEBUG_LED_INDEX
		Serial.print("ledIndex: ");
#endif
		for (uint8_t x = 0; x < GRID_WIDTH; x++) {

/*
#ifdef HACK_FOR_TALL_GRID
			uint32_t ledIndex = (x * GRID_HEIGHT) + y;
#else
			uint32_t ledIndex = (y * GRID_WIDTH) + x;
#endif
*/

#ifdef DEBUG_LED_INDEX
			if (x != 0) {
				Serial.print(", ");
			}
#endif


			uint16_t oneBasedLedIndex = ledGrid.retrieveLedNumberByPosition(x, y);
			if (oneBasedLedIndex == 0) {
#ifdef DEBUG_LED_INDEX
				Serial.print("N/A");
#endif
				continue;
			}
			uint32_t ledIndex = oneBasedLedIndex - 1;

#ifdef DEBUG_LED_INDEX
			Serial.print(ledIndex, DEC);
#endif
			currentAnimation->updateLedColor(x, y, rgbLedBuffer);

			leds.setPixel(
				ledIndex,
				rgbLedBuffer.rgb->r,
				rgbLedBuffer.rgb->g,
				rgbLedBuffer.rgb->b
			);
		}
#ifdef DEBUG_LED_INDEX
		Serial.println();
#endif
	}

	currentAnimation->frameEnd();

	leds.show();
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

    buttonProcessAction.setDelayMicros(1000 * 10); // must be less than the debounce delay
    buttonProcessAction.reset();

#ifdef DEBUG_BUTTON_TEST
    Serial.print("Press and release BACK, UP, DOWN then SELECT to begin");

    bool backPressed = false;
    bool upPressed = false;
    bool downPressed = false;
    bool selectPressed = false;

    while (!(backPressed && upPressed && downPressed && selectPressed)) {
        while(backButton.getValue()) { backPressed = true; Serial.print("B"); };
        while(upButton.getValue()) { upPressed = true; Serial.print("U");};
        while(downButton.getValue()) { downPressed = true; Serial.print("D");};
        while(selectButton.getValue()) { selectPressed = true; Serial.print("S");};
    }
#endif

    menuStack.initalize();

    Serial.println("FINISHED SETUP");

#ifndef USE_ANIMATOR
    resetAnimation();
#endif
}

void openNextAnimationFile() {

    bool opened = false;

    bool haveIssuedReset = false;

    do {
        opened = animationFile.openNext(root, O_RDONLY);
        if (!opened) {
            if (haveIssuedReset) {
                break;
            }
            resetSdCard();
            haveIssuedReset = true;
            continue;
        }

        animationFile.getFilename(fileName);
        Serial.print("Filename: ");
        Serial.print(fileName);
        if (strstr(fileName, ".LAX") == NULL) {
            animationFile.close();
            Serial.println("..SKIPPED");
            opened = false;
            continue;
        }

    } while (!opened);

    if (!opened) {
        Serial.print("No more animation files");
        return;
    }

    Serial.println("..OK");

    showFreeRam();


    fileReader.setSdFile(&animationFile);
    // setup leds and animation
    animator.readAnimationDetails(&fileReader);
    showFreeRam();

    animationScheduler.update();
}

void openNextAnimation() {
#ifdef USE_ANIMATOR
    openNextAnimationFile();
#else
    animationIndex++;
    if (animationIndex == TOTAL_ANIMATIONS) {
        animationIndex = 0;
    }
    resetAnimation();
#endif
}

void openPreviousAnimation() {
#ifdef USE_ANIMATOR
    openNextAnimationFile();
#else
    if (animationIndex == 0) {
        animationIndex = TOTAL_ANIMATIONS - 1;
    } else {
        animationIndex--;
    }
    resetAnimation();
#endif
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

    openSdCard();

    if (!isSdCardOpen) {
        return;
    }

#ifdef USE_ANIMATOR
    openNextAnimation();
#endif
}

void stopAnimation() {
    animationFile.close();
    animator.reset();
}

void onSdCardRemoved(void) {
    stopAnimation();
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

void waitForButtonRelease(DebouncedInput button) {
#ifdef DEBUG_BUTTON_RELEASE
        Serial.print(">");
#endif
    while (button.getValue()) {
#ifdef DEBUG_BUTTON_RELEASE
        Serial.print("-");
#endif
        delayMicroseconds(1000 * button.getDebounceDelayMillis());
    }
#ifdef DEBUG_BUTTON_RELEASE
    Serial.println("<");
#endif
}

void processButtons(void) {
    if (!buttonProcessAction.isActionDue()) {
        return;
    }
    menuStack.process();
}

void loop() {
    updateCpuActivityLed();
    updateSerialStatus();
    processGyro();
    checkSdCardStatus();
    processButtons();
#ifdef USE_ANIMATOR
    updateAnimation();
#else
    updateSimpleAnimation();
#endif
}

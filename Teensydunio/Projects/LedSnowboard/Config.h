/*
 * Config.h
 *
 *  Created on: 14 Jan 2014
 *      Author: hydra
 */

#ifndef CONFIG_H_
#define CONFIG_H_

/*
 ** SPI_MOSI - pin 11
 ** SPI_MISO - pin 12
 ** SPI_CLK - pin 13
*/
#define SD_CD_PIN 9 // Card detect for SD card
#define SD_CS 10 // Chip select for SD card

// Define pin to use to drive a status LED
#define TEENSY_LED_PIN 13
#define CPU_STATUS_LED_PIN 17

#define BACK_BUTTON_PIN 0
#define UP_BUTTON_PIN 1
#define DOWN_BUTTON_PIN 23
#define SELECT_BUTTON_PIN 22

// Select your animation - they are mutually exclusive.
//#define USE_ANIMATION_1
#define USE_ANIMATION_2

// hack to lower brightness by 50% to a reasonable level so the tester is not blinded
#define APPLY_BRIGHTNESS_HACK
//#define BRIGHTNESS_50_PERCENT
#define BRIGHTNESS_25_PERCENT

#ifdef BRIGHTNESS_25_PERCENT
#define BRIGHTNESS_MAX 0x40
#endif
#ifdef BRIGHTNESS_50_PERCENT
#define BRIGHTNESS_MAX 0x80
#endif

//
// Sampler
//
#define SAMPLE_FREQUENCY_HZ 50
#define SAMPLE_WINDOW 8
#define SAMPLE_STEP 4
//#define LOG_SAMPLE

//#define DEBUG_BUTTON_TEST

//#define OVERRIDE_ANIMATION_FREQUENCY

#define DEBUG_ANIMATOR_INITIALIZATION_OF_FUNCTION_DATA
//#define DEBUG_ANIMATOR_CODEC_LED_COLOURS
//#define DEBUG_ANIMATOR_CODEC_VALUE_AXIS
//#define DEBUG_ANIMATOR_FRAME
//#define DEBUG_ANIMATOR_CODEC_FINAL_INCREMENTS

// Uses more memory, but currently there may be memory memory errors if this is disabled.
#define USE_MULTIPLE_MALLOC_CALLS_FOR_MULTIDIMENSIONAL_ARRAYS

//#define DEBUG_ALL_ACCEL_DATA
//#define DEBUG_NORMALIZED_XY_ACCEL_DATA

#endif /* CONFIG_H_ */

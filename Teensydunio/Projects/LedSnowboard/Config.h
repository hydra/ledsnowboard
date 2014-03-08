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

#define LEDS_PER_STRIP 22
//#define HACK_MIRROR_LEDS

#define USE_ANIMATOR // Enable to use a codec based animator, disable to use a programmatic animator.

#define HACK_FOR_TALL_GRID
#ifdef HACK_FOR_TALL_GRID
// HACK old code was written for a TALL grid
#define GRID_HEIGHT LEDS_PER_STRIP
#define GRID_WIDTH 8
#else
#define GRID_HEIGHT 8
#define GRID_WIDTH LEDS_PER_STRIP
#endif

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
// Accelerometer
//

#define ACCEL_RANGE_MIN -10000
#define ACCEL_RANGE_MAX 10000
//
// Sampler
//
#define SAMPLE_FREQUENCY_HZ 100
#define SAMPLE_WINDOW 8
#define SAMPLE_STEP 4

//#define DEBUG_BUTTON_TEST
#define DEBUG_BUTTON_RELEASE

//#define OVERRIDE_ANIMATION_FREQUENCY

#define DEBUG_LED_INDEX_TEST (ledIndex == 0)
//#define DEBUG_ANIMATION_READER_POSITIONING
//#define DEBUG_ANIMATOR_INITIALIZATION_OF_FUNCTION_DATA
//#define DEBUG_ANIMATOR_CODEC_LED_COLOURS
//#define DEBUG_ANIMATOR_CODEC_VALUE_AXIS
//#define DEBUG_ANIMATOR_FRAME
//#define DEBUG_ANIMATOR_CODEC_FINAL_INCREMENTS
//#define DEBUG_ANIMATOR_FUNCTION_RANGES_INITIALSATION

// Uses more memory, but currently there may be memory memory errors if this is disabled.
#define USE_MULTIPLE_MALLOC_CALLS_FOR_MULTIDIMENSIONAL_ARRAYS

//#define DEBUG_ALL_ACCEL_DATA
//#define DEBUG_ACCEL_MIN_MAX

//#define DEBUG_SD_READER

#endif /* CONFIG_H_ */

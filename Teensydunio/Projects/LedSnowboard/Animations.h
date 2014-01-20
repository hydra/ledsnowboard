#ifndef ANIMATIONS_H_
#define ANIMATIONS_H_

#include "Config.h"

#ifdef USE_ANIMATION_1

#define COUNT_OF_LEDS_IN_ANIMATION 1
#define COUNT_OF_FUNCTIONS_IN_ANIMATION 3

#endif

#ifdef USE_ANIMATION_2
// FIXME doesn't fit in the FLASH.

#define COUNT_OF_LEDS_IN_ANIMATION 240
#define COUNT_OF_FUNCTIONS_IN_ANIMATION 10

#endif

extern const unsigned char animationData[];

#ifdef ANIMATION_IN_MEMORY
uint32_t getAnimationSizeInBytes(void);
#endif

#endif /* ANIMATIONS_H_ */

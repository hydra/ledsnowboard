#ifndef ANIMATIONS_H_
#define ANIMATIONS_H_

#include "Config.h"

#ifdef USE_ANIMATION_1

#define USE_OLD_FILE_FORMAT_WITH_ESCAPING

#endif

#ifdef USE_ANIMATION_2
// FIXME doesn't fit in the FLASH.

#endif

extern const unsigned char animationData[];

#endif /* ANIMATIONS_H_ */

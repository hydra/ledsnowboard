/*
 * animation.h
 *
 *  Created on: 14 Jan 2014
 *      Author: hydra
 */

#ifndef ANIMATION_H_
#define ANIMATION_H_

struct valueAxis {
    int lowValue;
    int highValue;
    int zeroValue;
} typedef ValueAxis;

// Frame Types
#define FT_COLOUR 1
#define FT_FUNCTION 2
#define FT_LINKED 3

#define INITIAL_LED 1

#define HEADER_BYTE 0x56
#define TERMINATING_BYTE 0x45
#define ESCAPE_BYTE 0x02
#define XOR_BYTE 0x20

void readAnimationDetails(void);
void renderNextFrame(void);

unsigned char readByteUnsignedChar(int* aPosition);
signed char readByteSignedChar(int* aPosition);
void readFunctionData(int num);
void readTimeAxis(void);
void readValueAxis(unsigned int valueAxisIndex);
void processFrame(unsigned int frameIndex);

#endif /* ANIMATION_H_ */

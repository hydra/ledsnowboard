/*
 * AnimationReader.cpp
 *
 *  Created on: 24 Jan 2014
 *      Author: Hydra
 */

#include "WProgram.h"

#include "Config.h"

#include "AnimationReader.h"

AnimationReader::AnimationReader(FileReader *fileReader) :
    position(0),
    fileReader(fileReader) {
}

void AnimationReader::seek(uint32_t _position) {
    position = _position;
    fileReader->seek(position);
}

uint32_t AnimationReader::getPosition(void) {
    return position;
}

#ifdef USE_OLD_FILE_FORMAT_WITH_ESCAPING

#define ESCAPE_BYTE 0x02
#define XOR_BYTE 0x20

uint8_t AnimationReader::readUnsignedByte() {
    unsigned char readByte = (unsigned char)fileReader->readByte();
    position++;

    if (readByte == ESCAPE_BYTE) {
        readByte = (unsigned char)fileReader->readByte();
        position++;
        readByte = readByte ^ XOR_BYTE;
    }

    return readByte;
}

int8_t AnimationReader::readSignedByte(void) {
    signed char readByte = (signed char)fileReader->readByte();
    position++;

    if (readByte == ESCAPE_BYTE) {
        readByte =  (signed char)fileReader->readByte();
        position++;
        readByte = readByte ^ XOR_BYTE;
    }

    return readByte;
}
#else
uint8_t AnimationReader::readUnsignedByte(void) {
    unsigned char readByte = (unsigned char)fileReader->readByte();
    position++;

    return readByte;
}

int8_t AnimationReader::readSignedByte(void) {
    signed char readByte = (signed char)fileReader->readByte();
    position++;

    return readByte;
}
#endif

uint32_t AnimationReader::readUnsignedInt32(void) {

    uint32_t result = readUnsignedByte();
    result |= readUnsignedByte() << 8;
    result |= readUnsignedByte() << 16;
    result |= readUnsignedByte() << 24;
    
    position += 4;

    return result;
}

/*
 * SdCardFileReader.cpp
 *
 *  Created on: 18 Jan 2014
 *      Author: Hydra
 */

#include "WProgram.h"

#include <SdFat.h>
#include <SdFatUtil.h>

#include "SdCardFileReader.h"

SdCardFileReader::SdCardFileReader() :
    sdFile(NULL),
    byteBuffer(0) { 
}

void SdCardFileReader::setSdFile(SdFile *_sdFile) {
    sdFile = _sdFile;
}

byte SdCardFileReader::readByte(void) {
    sdFile->read(&byteBuffer, 1);

    Serial.print("Read: ");
    Serial.print(byteBuffer, HEX);
    Serial.println();

    
    return byteBuffer;
}

void SdCardFileReader::seek(uint32_t position) {
    Serial.print("Seek: ");
    Serial.print(position, DEC);
    Serial.println();
    sdFile->seekSet(position);
}

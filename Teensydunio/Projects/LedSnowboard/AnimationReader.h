/*
 * AnimationReader.h
 *
 *  Created on: 24 Jan 2014
 *      Author: Hydra
 */

#ifndef ANIMATIONREADER_H_
#define ANIMATIONREADER_H_

#include "File/FileReader.h"

class AnimationReader {
public:
    AnimationReader(FileReader *fileReader);
    
    uint32_t readUnsignedInt32(void);
    uint8_t readUnsignedByte(void);
    int8_t readSignedByte(void);
    
    uint32_t getPosition(void);
    void seek(uint32_t position);
    
private:
    uint32_t position;
    FileReader *fileReader;
};

#endif /* ANIMATIONREADER_H_ */

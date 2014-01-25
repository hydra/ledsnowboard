/*
 * FileReader.h
 *
 *  Created on: 18 Jan 2014
 *      Author: Hydra
 */

#ifndef FILEREADER_H_
#define FILEREADER_H_

class FileReader {
public:
    virtual ~FileReader(void) = 0;
    virtual byte readByte(void) = 0;
    virtual void seek(uint32_t position) = 0;
};

#endif /* FILEREADER_H_ */

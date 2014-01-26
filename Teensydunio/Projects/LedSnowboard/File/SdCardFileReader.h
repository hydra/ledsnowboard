/*
 * SdCardFileReader.h
 *
 *  Created on: 18 Jan 2014
 *      Author: Hydra
 */

#ifndef SDCARDFILEREADER_H_
#define SDCARDFILEREADER_H_

class SdCardFileReader: public FileReader {
public:
    SdCardFileReader();
    virtual ~SdCardFileReader();
    void setSdFile(SdFile *sdFile);
    virtual byte readByte(void);
    virtual void seek(uint32_t position);
protected:
    SdFile *sdFile;
    byte byteBuffer;
};

#endif /* SDCARDFILEREADER_H_ */

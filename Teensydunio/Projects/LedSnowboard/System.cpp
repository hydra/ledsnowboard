/*
 * System.cpp
 *
 *  Created on: 22 Jan 2014
 *      Author: hydra
 */

#include "WProgram.h"
#include <SdFatUtil.h>


void systemHalt(void) {
    while (1) ;
}

uint32_t MyFreeRam(){ // for Teensy 3.0
    uint32_t stackTop;
    uint32_t heapTop;
    // current position of the stack.
    stackTop = (uint32_t) &stackTop;
    // current position of heap.
    void* hTop = malloc(1);
    heapTop = (uint32_t) hTop;
    free(hTop);
    // The difference is the free, available ram.
    return stackTop - heapTop;
}

void showFreeRam(void) {
    Serial.print("Free: 0x");
    Serial.print(MyFreeRam(), HEX);
    Serial.println();
}


bool isValidMemoryAddress(void *address) {
    return (address >= (void*)0x1fff8000L && address < (void*)(0x1fff8000L + 0x10000));
}

void verifyMemoryAllocation(void *address) {
    if (isValidMemoryAddress(address)) {
        return;
    }
    showFreeRam();

    Serial.print("!, failed memory allocation.  address: 0x");
    Serial.println((uint32_t)address, HEX);
    systemHalt();
}

/*
 * System.h
 *
 *  Created on: 22 Jan 2014
 *      Author: hydra
 */

#ifndef SYSTEM_H_
#define SYSTEM_H_

void systemHalt(void);
void showFreeRam(void);
bool isValidMemoryAddress(void *address);
void verifyMemoryAllocation(void *address);

#endif /* SYSTEM_H_ */

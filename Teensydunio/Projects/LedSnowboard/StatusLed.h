/*
 * StatusLed.h
 *
 *  Created on: 14 Jan 2014
 *      Author: hydra
 */

#ifndef STATUSLED_H_
#define STATUSLED_H_

class StatusLed {
public:
    StatusLed(unsigned int pin);

    void configure();

    void disable();
    void enable();
    void toggle();

private:
    bool state = false;
    unsigned int pin;
    void update();
};

#endif /* STATUSLED_H_ */

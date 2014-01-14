/*
 * StatusLed.cpp
 *
 *  Created on: 14 Jan 2014
 *      Author: hydra
 */

#include "WProgram.h"

#include "StatusLed.h"

StatusLed::StatusLed(unsigned int pin) :
    pin(pin)
{
    // TODO Auto-generated constructor stub

}

void StatusLed::configure() {
    disable();
}

void StatusLed::disable() {
    state = false;
    update();
}

void StatusLed::enable() {
    state = true;
    update();
}

void StatusLed::toggle() {
    state = !state;
    update();
}

void StatusLed::update() {
    digitalWrite(pin, state);
}

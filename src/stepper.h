#ifndef APPLE_II_STEPPER_H
#define APPLE_II_STEPPER_H

#include <stdint.h>

extern uint8_t phase[4];
extern int8_t track;

void phaseON(uint8_t i);
void phaseOFF(uint8_t i);

#endif
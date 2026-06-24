#include "stepper.h"

#include <stdio.h>

uint8_t phase[4] = {0};
uint8_t cog = 0;
extern int8_t track;

void phaseON(uint8_t i) {
    printf("phase %d ON\n", i);

    uint8_t step = 1;

    phase[i] = 1;

    if (cog == i);
    else if ((cog - i) == 1) {
        track--;
    }

    else if ((i - cog) == 1) {
        track++;
    }

    else if (i == 3) {
        track--;
    }

    else if (i == 0) {
        track++;
    }

    cog = i;

    if (track < 0) track = 0;
    if (track > 80) track = 80;

}

void phaseOFF(uint8_t i) {
    printf("phase %d OFF\n", i);
    phase[i] = 0;

    for (uint8_t n = 0; n < 4; n++) {
        if (phase[n]) i = n;
    }

    if (cog == i);
    else if ((cog - i) == 1) {
        track--;
    }

    else if ((i - cog) == 1) {
        track++;
    }

    else if (i == 3) {
        track--;
    }

    else if (i == 0) {
        track++;
    }

    cog = i;

    if (track < 0) track = 0;
    if (track > 80) track = 80;

}
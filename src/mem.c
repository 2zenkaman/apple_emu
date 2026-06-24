#include "mem.h"

#include "video.h"

#include "stepper.h"

extern uint8_t dump;
extern uint8_t MEM[MEMSIZ];
extern uint8_t TXT[TXTSIZ];

#define KBD     0xC000
#define KBDSTRB 0xC010
#define TXTCLR  0xC050
#define TXTSET  0xC051
#define MIXSET  0xC053
#define LORES   0xC056

#define IWM_PH0_OFF 0xC080
#define IWM_PH0_ON  0xC081

extern uint8_t mode;
extern uint8_t mixset;

extern uint8_t phase[4];

uint8_t* at(uint16_t i, uint8_t waccess) {
    if (i < RAMSIZ) return MEM + i;
    if (i > RAMSIZ + PERSIZ) return waccess ? &dump : MEM + i;

    switch (i) {
    case KBD:
        break;
    case KBDSTRB:
        MEM[KBD] = 0x00;
        break;
    case TXTCLR:
        mode = lores;
        break;
    case TXTSET:
        mode = text;
        mixset = 0;
        break;
    case MIXSET:
        mixset = 1;
        break;
    }

    for (uint8_t p = 0; p < 4; p++) {
        if (i == IWM_PH0_OFF + *at(0x2b, 0) + p) {
            phaseOFF(p);
        }

        if (i == IWM_PH0_ON + *at(0x2b, 0) + p) {
            phaseON(p);
        }
    }

    return MEM + i;
}

int loadrom(const char* path) {
    FILE* rom = fopen(path, "rb");
    fseek(rom, 0L, SEEK_END);
    long romsize = ftell(rom);
    rewind(rom);

    size_t nbytes = fread(MEM + (MEMSIZ - romsize), 1, romsize, rom);
    fclose(rom);
}
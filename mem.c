#include "mem.h"

#include "video.h"

extern uint8_t dump;
extern uint8_t MEM[MEMSIZ];
extern uint8_t TXT[TXTSIZ];

const uint16_t KBD     = 0xC000;
const uint16_t KBDSTRB = 0xC010;
const uint16_t TXTCLR  = 0xC050;
const uint16_t TXTSET  = 0xC051;
const uint16_t MIXSET  = 0xC053;
const uint16_t LORES   = 0xC056;

extern uint8_t mode;
extern uint8_t mixset;

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

    return MEM + i;
}
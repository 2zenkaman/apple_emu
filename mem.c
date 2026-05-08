#include "mem.h"

extern uint8_t dump;
extern uint8_t MEM[MEMSIZ];
extern uint8_t TXT[TXTSIZ];

uint8_t* at(uint16_t i, uint8_t waccess) {
    if (i < RAMSIZ + PERSIZ) return MEM + i;
    return waccess ? &dump : MEM + i;
}
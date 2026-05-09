#include "video.h"

#include "mem.h"

extern uint8_t MEM[MEMSIZ];
extern uint8_t factor;

extern uint8_t mode;
extern uint8_t mixset;

// routine borrowed from original autostart rom
uint16_t bascalc(uint8_t A) {
                uint8_t bas[2];
                uint8_t carry = A & 1;
/*BASCALC:*/    uint8_t temp = A;       // pha
                A >>= 1;                // lsr A
                A &= 0x3;               // and #$03
                A |= 0x4;               // ora #$04
                bas[1] = A;             // sta BASH
                A = temp;               // pla
                A &= 0x18;              // and #$18
                if (carry)              // bcc BASCLC2
                    A += 0x7f + carry;  // adc #$7f
/*BASCLC2*/     bas[0] = A;             // sta BASL
                A <<= 2;                // asl A
                                        // asl A
                bas[0] |= A;            // ora BASL
                                        // sta BASL
                return *(uint16_t*)bas; // rts
}

void renderchar(uint16_t x, uint16_t y) {
    uint16_t basaddr = bascalc(y) + x;

    uint8_t c = *at(basaddr, 0) & 0x7f;
    if (c < 32 || c > 95) return;

    SDL_Rect cell;
    cell.w = 7 * factor;
    cell.h = 8 * factor;
    cell.x = x * cell.w;
    cell.y = y * cell.h;

    SDL_RenderCopy(renderer, textt[c-32], NULL, &cell);
}

void renderlores(uint16_t x, uint16_t y) {
    uint16_t basaddr = bascalc(y) + x;

    uint8_t colordata = *at(basaddr, 0);

    SDL_Rect cell;
    cell.w = 7 * factor;
    cell.h = 4 * factor;
    cell.x = x * cell.w;
    cell.y = y * cell.h * 2;

    SDL_RenderCopy(renderer, lorest[colordata & 0x0f], NULL, &cell);

    cell.y += cell.h;
    SDL_RenderCopy(renderer, lorest[colordata >> 4], NULL, &cell);
}

void display() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);

    for (size_t y = 0; y < 24; y++) {
        for (size_t x = 0; x < 40; x++) {
            if (mixset && y > 19) {
                renderchar(x, y);
                continue;
            }

            switch (mode) {
            case lores:
                renderlores(x, y);
                break;
            case text:
                renderchar(x, y);
                break;
            }
        }
    }

    SDL_RenderPresent(renderer);
}
#include "video.h"

#include "mem.h"

extern uint8_t MEM[MEMSIZ];
extern uint8_t factor;

void renderchar(uint16_t x, uint16_t y, uint8_t c) {
    if (c < 32 || c > 95) return;

    SDL_Rect cell;
    cell.w = 7 * factor;
    cell.h = 8 * factor;
    cell.x = x * 7 * factor;
    cell.y = y * 8 * factor;

    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderCopy(renderer, font[c-32], NULL, &cell);
}

void display() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);

    uint8_t* rows[] = {MEM + 0x400, MEM + 0x480, MEM + 0x500, MEM + 0x580, MEM + 0x600, MEM + 0x680, MEM + 0x700, MEM + 0x780};

    for (size_t y = 0; y < 24; y++) {
        uint8_t lnum = y % 8;
        uint8_t offset = y / 8 * 0x28;
        uint8_t* segment = rows[lnum] + offset;
        for (size_t x = 0; x < 40; x++) {
            char curr = segment[x] & 0x7F; // ASCII
            renderchar(x, y, curr);
        }
    }

    SDL_RenderPresent(renderer);
}
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <SDL2/SDL.h>

#include "mem.h"
#include "cpu.h"
#include "video.h"

const char* const romname = "./rom/rom.bin";
const char* const txtname = "./rom/character.bin";


SDL_Window*   window;
SDL_Renderer* renderer;
SDL_Texture* font[CHARSET];
uint8_t mode = TEXT;


uint8_t dump;
uint8_t MEM[MEMSIZ] = {0};
uint8_t TXT[TXTSIZ] = {0};


uint8_t A = 0;
uint8_t X = 0;
uint8_t Y = 0;

uint16_t PC = 0;
uint8_t  SP = 0xFF;

uint8_t C = 0;
uint8_t Z = 0;
uint8_t I = 0;
uint8_t D = 0;
uint8_t B = 0;
uint8_t V = 0;
uint8_t N = 0;


const uint8_t factor = 4;


int main(int argc, char* argv[]) {

    // SDL INIT
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        puts(SDL_GetError());
        return 1;
    }

    window = SDL_CreateWindow("Apple ][", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 280 * factor, 192 * factor, 0);
    if (window == NULL) {
        puts(SDL_GetError());
        return 1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        puts(SDL_GetError());
        return 1;
    }


    // ROM LOADING
    {
        FILE* rom = fopen(romname, "rb");
        fseek(rom, 0L, SEEK_END);
        long romsize = ftell(rom);
        rewind(rom);
        
        size_t nbytes = fread(MEM + (MEMSIZ - romsize), 1, romsize, rom);
        fclose(rom);
    }


    // TEXT ROM LOADING
    {
        FILE* rom = fopen(txtname, "rb");
        fread(TXT, 2 KB, 1, rom);
        fclose(rom);

        const uint32_t white = 0xffffffff;
        const uint32_t black = 0x00000000;
        
        SDL_Surface* c = SDL_CreateRGBSurface(0, 7, 8, 32, 0, 0, 0, 0);
        
        for (uint8_t i = 0; i < CHARSET; i++) {
            uint8_t* chardata = TXT + i*8; // 8 bytes per char
            if (i / 32 == 0) {
                chardata += 256;
            } else {
                chardata -= 256;
            }

            SDL_FillRect(c, NULL, 0x00000000);

            for (uint8_t y = 0; y < 8; y++) {
                uint8_t pixrow = chardata[y];
                for (uint8_t x = 1; x < 8; x++) {
                    SDL_Rect pix;
                    pix.w = pix.h = 1;
                    pix.x = x;
                    pix.y = y;
                    SDL_FillRect(c, &pix, pixrow & (0x80 >> x) ? white : black);
                }
            }

            font[i] = SDL_CreateTextureFromSurface(renderer, c);
        }
    }


    printf("MPU is reset to %X\n", reset());

    uint8_t running = 1;
    SDL_Event e;
    while (running) {
        while (SDL_PollEvent(&e)) {
            switch (e.type)
            {
            case SDL_QUIT:
                return 0;
            case SDL_TEXTINPUT:
                printf("%c\n", (char)e.text.text[0]);
                *at(0xC000, 1) = (char)e.text.text[0] ^ 0x80;
                break;
            case SDL_KEYDOWN:
                *at(0xC000, 1) = (char)e.key.keysym.sym ^ 0x80;
                // if (e.key.keysym.sym == SDLK_4) {
                //     int fd = open("dump.bin", O_WRONLY);
                //     write(fd, mem.ram, 12 * 1024);
                //     close(fd);
                // }
                break;
            default:
                break;
            }
        }

        for (size_t i = 0; i < 100000; i++) {
            exec();
        }

        display();
    }
}
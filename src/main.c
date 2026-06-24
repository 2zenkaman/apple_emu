#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <SDL2/SDL.h>

#include "mem.h"
#include "cpu.h"
#include "video.h"

const char* romname = "./rom.bin";
const char* txtname = "./character.bin";

SDL_Window*   window;
SDL_Renderer* renderer;
SDL_Texture* textt[64];
SDL_Texture* lorest[16];
uint8_t mode = text;
uint8_t mixset = 0;


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

uint8_t floppy[40][16][400] = {0};
int8_t track = 0;
uint16_t flptr = 0;


const uint8_t factor = 3;


int main(int argc, char* argv[]) {
    // if (argc != 2) return 1; 

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
        if (!rom) {
            perror("file could not open");
            return 1;
        }

        fseek(rom, 0L, SEEK_END);
        long romsize = ftell(rom);
        rewind(rom);
        
        size_t nbytes = fread(MEM + (MEMSIZ - romsize), 1, romsize, rom);
        fclose(rom);
    }


    // TEXT ROM LOADING
    {
        FILE* rom = fopen(txtname, "rb");
        if (!rom) {
            perror("file could not open");
            return 1;
        }

        fread(TXT, 2 KB, 1, rom);
        fclose(rom);

        const uint32_t white = 0xffffffff;
        const uint32_t black = 0x00000000;
        
        SDL_Surface* c = SDL_CreateRGBSurface(0, 7, 8, 32, 0, 0, 0, 0);
        
        for (uint8_t i = 0; i < 64; i++) {
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

            textt[i] = SDL_CreateTextureFromSurface(renderer, c);
        }

        SDL_FreeSurface(c);
    }


    // FLOPPY
    // {
    //     FILE* fptr = fopen(argv[1], "rb");
    //     fseek(fptr, 0L, SEEK_END);
    //     rewind(fptr);

    //     for (uint8_t t = 0; t < 40; t++) {
    //         for (uint8_t s = 0; s < 16; s++) {
    //             fread(floppy[t][s], 400, 1, fptr);
    //         }
    //     }
        
    //     fclose(fptr);
    // }


    // {
    //     FILE* BOOT0 = fopen("rom/C600ROM", "r");
    //     fread(MEM + 0xC600, 1, 256, BOOT0);
    //     fclose(BOOT0);
    // }


    // LORES COLOR TEXTURES
    {
        SDL_Surface* s = SDL_CreateRGBSurface(0, 7, 4, 32, 0, 0, 0, 0);

        uint32_t colors[16] = {SDL_MapRGB(s->format, 0x00, 0x00, 0x00), SDL_MapRGB(s->format, 0xB0, 0x02, 0x58), SDL_MapRGB(s->format, 0x1E, 0x2B, 0xFF), SDL_MapRGB(s->format, 0xCE, 0x2D, 0xFF), SDL_MapRGB(s->format, 0x00, 0x7E, 0x28), SDL_MapRGB(s->format, 0x80, 0x80, 0x80), SDL_MapRGB(s->format, 0x00, 0xA9, 0xFF), SDL_MapRGB(s->format, 0x9E, 0xAB, 0xFF), SDL_MapRGB(s->format, 0x61, 0x54, 0x00), SDL_MapRGB(s->format, 0xFF, 0x65, 0x00), SDL_MapRGB(s->format, 0x80, 0x80, 0x80), SDL_MapRGB(s->format, 0xFF, 0x81, 0xD7), SDL_MapRGB(s->format, 0x31, 0xD2, 0x00), SDL_MapRGB(s->format, 0xE1, 0xD4, 0x00), SDL_MapRGB(s->format, 0x4F, 0xFD, 0xA7), SDL_MapRGB(s->format, 0xFF, 0xFF, 0xFF)};

        for (uint8_t i = 0; i < 16; i++) {
            SDL_FillRect(s, NULL, colors[i]);
            lorest[i] = SDL_CreateTextureFromSurface(renderer, s);
        }
        SDL_FreeSurface(s);
    }


    printf("MPU is reset to %X\n", reset());

    uint8_t running = 1;

    SDL_Event e;
    while (running) {
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
            case SDL_QUIT:
                running = 0;
            case SDL_TEXTINPUT:
                *at(0xC000, 1) = (char)e.text.text[0] ^ 0x80;
                break;
            case SDL_KEYDOWN:
                *at(0xC000, 1) = (char)e.key.keysym.sym ^ 0x80;
                break;
            default:
                break;
            }
        }

        for (size_t i = 0; i < 1000000; i++) {
            exec();
        }

        display();
    }

    return 0;
}
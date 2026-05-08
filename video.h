#ifndef APPLE_II_VIDEO_H
#define APPLE_II_VIDEO_H

#include <stdint.h>

#include <SDL2/SDL.h>

#define CHARSET 64

extern SDL_Window*   window;
extern SDL_Renderer* renderer;

extern SDL_Texture* font[CHARSET];

void renderchar(uint16_t x, uint16_t y, uint8_t c);
void display();

#endif
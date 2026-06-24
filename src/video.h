#ifndef APPLE_II_VIDEO_H
#define APPLE_II_VIDEO_H

#include <stdint.h>

#include <SDL2/SDL.h>

extern SDL_Window*   window;
extern SDL_Renderer* renderer;

extern SDL_Texture* textt[64];
extern SDL_Texture* lorest[16];

enum Mode {
    text,
    lores,
    hires,
};

void display();

#endif
#pragma once

#include "SDL3/SDL.h"
#include "cell.h"

typedef struct SDLState {
    SDL_Window* win;
    SDL_Surface* surf;
    int w;
    int h;
} SDLState;

SDLState initSDL(int w, int h);

void display(const SDLState* surface, const CellularAutomaton* automaton);

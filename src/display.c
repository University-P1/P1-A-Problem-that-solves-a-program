#include "display.h"
#include "SDL3/SDL.h"
#include "SDL3/SDL_error.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_main.h"
#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_surface.h"
#include "SDL3/SDL_video.h"
#include <SDL3/SDL_rect.h>
#include <stdlib.h>
#include "cell.h"

int min(int a, int b) {
    if (a < b)
        return a;
return b;
}

void drawCell(const CellularAutomaton* automaton, size_t row, size_t col, void* userdata) {
    SDLState* state = userdata;
    const Cell cell = automaton->rows[row].elements[col];

    int w;
    int h;
    if (!SDL_GetWindowSize(state->win, &w, &h)) {
        SDL_Log("SDL Error, Couldn't get windowsize\nmsg: %s", SDL_GetError());
        return;
    }

    const int min_size = min(w, h);
    const int num_cols = (int)automaton->rows[0].count;
    const int num_rows = (int)automaton->num_rows;
    const int cell_width = min_size / num_cols;
    const int cell_height = min_size / num_rows;

    SDL_Rect rect = {
        .x = cell_width * (int)col,
        .y = cell_height * (int)row,
        .w = cell_width,
        .h = cell_height,
    };

    Uint8 r = 0;
    Uint8 g = 0;
    Uint8 b = 0;

    switch (cell.state) {
    case CELLSTATE_NORMAL:
        g = 255;
        break;
    case CELLSTATE_ONFIRE:
        r = 255;
        break;
    case CELLSTATE_BURNT:
      break;
    }

    SDL_FillSurfaceRect(state->surf, &rect, SDL_MapSurfaceRGB(state->surf, r, g, b));
}

void display(const SDLState* state, const CellularAutomaton* automaton) {
    SDL_ClearSurface(state->surf, 1, 1, 1, 1);

    // Draw the cells
    forEachCell(automaton, drawCell, (void*)state);
    //drawCell(automaton, 0, 0, (void*)state);

    // SDL_Rect rect = {
    //     .x = 0,
    //     .y = 0,
    //     .w = 100,
    //     .h = 100,
    // };
    //
    // SDL_FillSurfaceRect(state->surf, &rect, SDL_MapSurfaceRGB(state->surf, 255, 0, 0));

    SDL_UpdateWindowSurface(state->win);
}

SDLState initSDL(int w, int h) {
    SDLState res = {
        .win = nullptr,
        .surf = nullptr,
        .w = w,
        .h = h,
    };

    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log( "SDL could not initialize! SDL error: %s\n", SDL_GetError() );
        return res;
    }

    SDL_Window* win = SDL_CreateWindow( "SDL3 Tutorial: Hello SDL3", w, h, 0 );

    if (win == nullptr)
    {
        SDL_Log( "Window could not be created! SDL error: %s\n", SDL_GetError() );
        return res;
    }

    SDL_Surface* surf = SDL_GetWindowSurface(win);
    if (surf == nullptr) {
        SDL_Log( "Surface could not be created! SDL error: %s\n", SDL_GetError() );
        return res;
    }

    res.win = win;
    res.surf = surf;
    return res;
}


#include "display.h"
#include "SDL3/SDL_error.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_surface.h"
#include "SDL3/SDL_video.h"
#include <SDL3/SDL_rect.h>
#include <assert.h>
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

    typedef struct Color {
        Uint8 r;
        Uint8 g;
        Uint8 b;
    } Color;
    Color c = {0, 0, 0};

    if (cell.state == CELLSTATE_ONFIRE) {
        c = (Color){255, 0, 0};
    } else if (cell.state == CELLSTATE_NORMAL) {
        switch (cell.type) {
        case VEG_BROADLEAVES:
            c = (Color){0, 0, 255}; // blue
            break;
        case VEG_SHRUBS:
            c = (Color){0, 100, 0}; // dark green
            break;
        case VEG_GRASSLAND:
            c = (Color){144, 238, 144}; // light green
            break;
        case VEG_FIREPRONE:
            c = (Color){235, 65, 65}; // red
            break;
        case VEG_AGROFORESTRY:
            c = (Color){255, 255, 0}; // yellow
            break;
        case VEG_NOTFIREPRONE:
            c = (Color){138, 138, 138}; // grey
            break;
        default:
            assert(false && "Invalid vegtype");
        }
    }

    SDL_FillSurfaceRect(state->surf, &rect, SDL_MapSurfaceRGB(state->surf, c.r, c.g, c.b));
}

void display(const SDLState* state, const CellularAutomaton* automaton) {
    SDL_ClearSurface(state->surf, 1, 1, 1, 1);

    // Draw the cells
    forEachCell(automaton, drawCell, (void*)state);

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


#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_surface.h"
#include "SDL3/SDL_video.h"
#include "cell.h"
#include "display.h"
#include "input.h"
#include "direct_spread.h"
#include "spotting_spread.h"
#include "burnout_cell.h"
#include "wchar.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

time_t time_diff(struct timeval *start, struct timeval *end) {
  return (end->tv_sec - start->tv_sec) * 1000000l + (end->tv_usec - start->tv_usec);
}

int main(int argc, char const* const* argv) {
    // Random seed for the random function
    srand((unsigned int)time(nullptr));

    if (argc != 2) {
        fputs("ERROR: Too many or too little arguments", stderr);
        exit(EXIT_FAILURE);
    }

    const char* file_path = argv[1];
    CellularAutomaton automaton = readInitialState(file_path);
    if (automaton.num_rows == 0) {
        fputs("We failed creating the automaton from the input file :(\n", stderr);
        return EXIT_FAILURE;
    }

    SDLState state = initSDL(16 * 80, 9 * 80);
    if (state.win == nullptr) {
        return 1;
    }

    int step;

    fprintf(stderr, "How many times do you wish for the simulation to run?");
    if (scanf("%d", &step) != 1) { // fixed '< 0' to '!= 1'
        fputs("We failed reading... whut.. :(\n", stderr);
        return EXIT_FAILURE;
    }

    bool running = true;
    int i = 0;
    struct timeval begin;
    gettimeofday(&begin, NULL);
    while (running) {
        // handle SDL input
        SDL_Event event;
        SDL_zero(event);

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT)
                running = false;
        }

        if (i >= step)
            continue;


        struct timeval end;
        gettimeofday(&end, NULL);
        if (time_diff(&begin, &end) < 100000l)
            continue;

        gettimeofday(&begin, NULL);

        display(&state, &automaton);

        // Spread fire
        CellularAutomaton new = directSpread(&automaton);
        destroyAutomaton(&automaton);
        automaton = new;

        // Spread fire via spotting
        new = spottingSpread(&automaton);
        destroyAutomaton(&automaton);
        automaton = new;

        // Burn cells based on heal / fuel left
        new = burnoutCells(&automaton);
        destroyAutomaton(&automaton);
        automaton = new;

        i++;
    }

    destroyAutomaton(&automaton);

    SDL_DestroySurface(state.surf);
    SDL_DestroyWindow(state.win);
    SDL_Quit();
    return 0;

}


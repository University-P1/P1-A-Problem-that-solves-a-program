#include <stdio.h>
#include <stdlib.h>
#include "cell.h"
#include "input.h"
#include "direct_spread.h"
#include "spotting_spread.h"
#include "burnout_cell.h"

int main(int argc, char const* const* argv) {
    if (argc != 2) {
        fputs("ERROR: Too many or too little arguments", stderr);
        exit(EXIT_FAILURE);
    }

    const char* file_path = argv[1];
    const CellularAutomaton automaton = readInitialState(file_path);


    int step;
    scanf("%d" ,&step);
    printf("Indtast hvor mange gange simulationen skal køre");

    for (int i = 0; i < step; i++) {
        // Spread fire
        directSpread(&automaton);

        // Spread fire via spotting
        spottingSpread(&automaton);

        // Burn cells based on heal / fuel left
        burnoutCells(&automaton);
    }
    return 0;

}


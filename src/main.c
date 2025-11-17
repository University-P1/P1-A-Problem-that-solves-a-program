#include <stdio.h>
#include <stdlib.h>
#include "cell.h"
#include "input.h"

int main(int argc, char const* const* argv) {
    if (argc != 2) {
        fputs("ERROR: Too many or too little arguments", stderr);
        exit(EXIT_FAILURE);
    }
    const char* file_path = argv[1];
    const CellularAutomaton automaton = readInitialState(file_path);
    printf("num_rows: %zu\n", automaton.num_rows);
    if (automaton.num_rows > 0) printf("row width: %zu\n", automaton.rows[0].count);
    printf("Wind speed X: %f\n", automaton.windX);
    printf("Wind speed Y: %f\n", automaton.windY);


    for (size_t row = 0; row < automaton.num_rows; row++) {
        const CellArray arr = automaton.rows[row];
        for (size_t col = 0; col < arr.count; col++) {
            const Cell* cell = arr.elements + col;
            printCell(cell, stderr);
        }
    }
}

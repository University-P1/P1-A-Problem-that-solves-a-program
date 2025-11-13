#include "direct_spread.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

void spreadToNeighbours(CellularAutomaton* automaton, size_t row, size_t col);
float chanceToSpread(const Cell* src, const Cell* dst);

/// Modifies the Cellular Automaton by spreading the fire between cells
void directSpread(CellularAutomaton* automaton) {
    // we iterate through our grid of cells
    for (size_t row = 0; row < automaton->num_rows; row++ ) {
        CellArray cell_arr = automaton->rows[row];
        for (size_t col = 0; col < cell_arr.count; col++ ) {
            Cell cell = cell_arr.elements[col];
            if (cell.state != CELLSTATE_ONFIRE) {
                continue;
            }
            // We know the cell is on fire
            // We are looping over neighbouring cells to check if they are gonna catch fire
            spreadToNeighbours(automaton, row, col);

        }

    }
}

void spreadToNeighbours(CellularAutomaton* automaton, size_t row, size_t col) {
    // input validation
    assert(row < automaton->num_rows && "out of bounds");
    CellArray cells = automaton->rows[row];

    assert(col < cells.count && "out of bounds");
    Cell spreading_cell = cells.elements[col];

    // Looping over neighbouring cells
    for (int neighbour_row = (int)row - 1; neighbour_row < row + 1; neighbour_row++ ) {
        // if row is out of bounds = skip
        if (neighbour_row < 0 ) {
            continue;
        }
        if (neighbour_row >= automaton->num_rows) {
            break;
        }
        CellArray neighbour_cell_arr = automaton->rows[neighbour_row];

        for (int neighbour_col = (int)col - 1; neighbour_col < col + 1; neighbour_col++) {
            // if column is out of bounds = skip
            if (neighbour_col < 0) {
                continue;
            }
            if (neighbour_col >= cells.count) {
                break;
            }
            Cell* neighbouring_cell = &neighbour_cell_arr.elements[neighbour_col];

            if (neighbouring_cell->state != CELLSTATE_NORMAL) {
                continue;
            }

            // calculating the chance the spreading cell will ignite the neighbouring cell
            float chance = chanceToSpread(&spreading_cell, neighbouring_cell);
            // Generating a random number between 1 and 0, if the number i less than the chance, the fire will spread.
            float randnum = (float)rand() / (float)RAND_MAX;
            if (randnum >= chance) {
                continue;
            }

            // The fire spreads to the cell :)
            neighbouring_cell->state = CELLSTATE_ONFIRE;
        }

    }
}

float chanceToSpread(const Cell* src,  const Cell* dst) {
    return 0.1f;
}
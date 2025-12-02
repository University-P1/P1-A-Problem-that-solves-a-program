#include "direct_spread.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

void spreadToNeighbours(const CellularAutomaton* automaton, size_t row, size_t col);
float chanceToSpread(const Cell* src, const Cell* dst);

/// Modifies the Cellular Automaton by spreading the fire between cells
void directSpread(const CellularAutomaton* automaton) {
    // we iterate through our grid of cells
    for (size_t row = 0; row < automaton->num_rows; row++ ) {
        CellArray cell_arr = automaton->rows[row];
        for (size_t col = 0; col < cell_arr.count; col++ ) {
            Cell cell = cell_arr.elements[col];
            if (cell.state != CELLSTATE_ONFIRE) {
                continue;
            }
            // We know the cell is on fire
            // We are looping over neighbouring cells to check if they are going to catch fire
            spreadToNeighbours(automaton, row, col);

        }

    }
}

void spreadToNeighbours(const CellularAutomaton* automaton, size_t row, size_t col) {
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

float chanceToSpread(const Cell* src, const Cell* dst) {

    // tabal of nominal fire probability from source https://www.mdpi.com/2571-6255/3/3/26
    float nominals[VEG_LAST][VEG_LAST] = {
        //   B      S     G     FP     AF   N
        {.3f, .375f, .25f, .275f, .25f, .25f},      // B
        {.375f, .375f, .475f, .4f, .3f, .475f},     // S
        {.45f, .475f, .475f, .475f, .375f, .475f},  // G
        {.225f, .325f, .25f, .35f, .2f, .35f},      // FP
        {.25f, .25f, .3f, .475f, .35f, .25f},       // AF
        {.075f, .1f, .075f, .275f, .075f, .075f},   // N
    };

    // nominal fire probability
    float p_n = nominals[src->type][dst->type];

    //moisture level from 1-100 in the cell we are trying to spread to
    float m_l = dst->moisture;
    // moisture level factor, the higher the moisture level, the more negative it becomes, therefore lower chance
    // chance of spreading
    float e_m = expf(-0.014f * m_l);

    // wind and slope factor set to 1 for now
    float a_wh = 1.0f;

    // algorith from source, spread probability from burning cell to neighbuoring cell
    float p_burn = (1 - powf(1 - p_n,a_wh)) * e_m;

    return p_burn;
}
#include "spotting_spread.h"
#include "direct_spread.h"
#include "cell.h"
#include <stdio.h>
#include <stdlib.h>

static bool throwsFirebrand(const CellularAutomaton* automaton, size_t row, size_t col);

/// Modifies the Cellular Automaton by spreading the fire via spotting
CellularAutomaton spottingSpread(const CellularAutomaton* automaton) {
    CellularAutomaton new_automaton = cloneAutomaton(automaton);

    for (size_t row = 0; row < automaton->num_rows; row++ ) {
        CellArray cell_arr = automaton->rows[row];

        for (size_t col = 0; col < cell_arr.count; col++ ) {
            Cell cell = cell_arr.elements[col];
            if (cell.state != CELLSTATE_ONFIRE)
                continue;

            if (!throwsFirebrand(automaton, row, col))
                continue;

            // Try and throw firebrand here:
            int travel_distance = 0;
            switch (automaton->speed) {
            case WIND_NONE:
                travel_distance = 1;
                break;
            case WIND_SLOW:
                travel_distance = 4;
                break;
            case WIND_MODERATE:
                travel_distance = 7;
                break;
            case WIND_FAST:
                travel_distance = 12;
                break;
            case WIND_EXTREME:
                travel_distance = 16;
                break;
            default:
                assert(false && "Invalid windspeed encountered");
            }

            const int dst_col = (int)col + (travel_distance * automaton->windX);
            const int dst_row = (int)row + (travel_distance * automaton->windY);

            // outside the simulation space
            if (dst_col < 0 || dst_col >= (int)cell_arr.count)
                continue;

            if (dst_row < 0 || dst_row >= (int)automaton->num_rows)
                continue;

            const Cell dst_cell = automaton->rows[dst_row].elements[dst_col];

            // Chance to spread to the cell, but via wifi instead of cable
            const float p = chanceToSpread(&cell, &dst_cell, 1.f);

            // Determine if the chance succeeds
            const float determinator = (float)rand() / (float)RAND_MAX;
            if (determinator >= p)
                continue;

            // We are spreading to a cell!
            new_automaton.rows[dst_row].elements[dst_col].state = CELLSTATE_ONFIRE;
        }
    }

    return new_automaton;
}


static bool throwsFirebrand(const CellularAutomaton* automaton, size_t row, size_t col) {
    const Cell cell = automaton->rows[row].elements[col];

    unsigned int burning_neighbors = 0;

    // Count number of burning neighbors
    for (size_t neighbor_row = row - 1; neighbor_row <= row + 1; neighbor_row++ ) {
        if (neighbor_row < 0 || neighbor_row >= automaton->num_rows)
            continue;

        CellArray neighbor_row_arr = automaton->rows[neighbor_row];

        for (size_t neighbour_col = col - 1; neighbour_col <= col + 1; neighbour_col++) {
            if (neighbour_col < 0 || neighbour_col >= neighbor_row_arr.count)
                continue;

            Cell cell = neighbor_row_arr.elements[col];
            burning_neighbors += cell.state == CELLSTATE_ONFIRE;
        }
    }

    const float base_prop = .001f; // pulled from ass

    const float neighbor_factor = (float)burning_neighbors * 0.1f; // pulled from ass
    const float wind_factor = (float)automaton->speed + 1.f; // pulled from ass
    const float moisture_factor = 1.f - cell.moisture; // linear because I said so

    // Chance that it throws a firebrand
    const float p = base_prop * neighbor_factor * wind_factor * moisture_factor;

    // Evaluate said chance with random number from 0.f to 1.f
    const float determinator = (float)rand() / (float)RAND_MAX;
    return determinator < p;
}

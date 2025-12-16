#include "spotting_spread.h"
#include "cell.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

static bool throwsFirebrand(const CellularAutomaton* automaton, size_t row, size_t col);
static float ignitionSpotting(float distance, const Cell* dst_cell);


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
            float temp_distance = 0.0f;
            switch (automaton->speed) {
            case WIND_NONE:
                temp_distance = 1.0f;
                break;
            case WIND_SLOW:
                temp_distance = 4.0f;
                break;
            case WIND_MODERATE:
                temp_distance = 7.0f;
                break;
            case WIND_FAST:
                temp_distance = 12.0f;
                break;
            case WIND_EXTREME:
                temp_distance = 16.0f;
                break;
            default:
                assert(false && "Invalid windspeed encountered");
            }

            // implementer turbulens
            float sigma = temp_distance * 0.3f;
            float stochastic_value = ((float)rand() / (float)RAND_MAX) - 0.5f; // -0.5 til 0.5
            float total_distance = temp_distance + sigma * stochastic_value * 2.0f;

            const int dst_col = (int)col + ((int)roundf(total_distance) * automaton->windX);
            const int dst_row = (int)row + ((int)roundf(total_distance) * automaton->windY);

            // outside the simulation space
            if (dst_col < 0 || dst_col >= (int)cell_arr.count)
                continue;

            if (dst_row < 0 || dst_row >= (int)automaton->num_rows)
                continue;

            const Cell dst_cell = automaton->rows[dst_row].elements[dst_col];

            // chance to spread to cell (with decay)
            const float p = ignitionSpotting(total_distance, &dst_cell);
            // determine if succeeds
            const float determinator = (float)rand() / (float)RAND_MAX;
            if (determinator >= p)
                continue;

            // We are spreading to a cell!
            new_automaton.rows[dst_row].elements[dst_col].state = CELLSTATE_ONFIRE;
        }
    }

    return new_automaton;
}


// chance to spread to cell with cell decay
static float ignitionSpotting(float total_distance, const Cell* dst_cell) {
    const float p0 = 0.5f;
    const float k  = 0.1f;

    const float receptivity = 1.0f - dst_cell->moisture;
    if (receptivity <= 0.0f)
        return 0.0f;

    const float p = p0 * expf(-k * total_distance) * receptivity;

    return fminf(fmaxf(p, 0.0f), 1.0f);
}


static bool throwsFirebrand(const CellularAutomaton* automaton, size_t row, size_t col) {
    const Cell cell = automaton->rows[row].elements[col];

    // Count number of burning neighbors
    unsigned int burning_neighbors = 0;
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

    const float base_prop = .001f;

    const float neighbor_factor = (float)burning_neighbors * 0.1f;
    const float wind_factor = (float)automaton->speed + 1.f;
    const float moisture_factor = 1.f - cell.moisture; // Linear

    // Chance that it throws a firebrand
    const float p = base_prop * neighbor_factor * wind_factor * moisture_factor;

    // Evaluate said chance with random number from 0.f to 1.f
    const float determinator = (float)rand() / (float)RAND_MAX;
    return determinator < p;
}

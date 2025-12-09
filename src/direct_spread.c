#include "direct_spread.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

void spreadToNeighbors(const CellularAutomaton* automaton, size_t row, size_t col);
float chanceToSpread(const Cell* src, const Cell* dst, float a_w);

float wind_effect_table[5][5] = {
    // Chatgpt read the graph, check if they are correct.
    // Wind factor, effect of wind and direction between the neighboring cell and the burning cell.
    //   0°     45°    90°    135°   190°
    {1.20f, 1.05f, 1.00f, 1.00f, 1.00f}, // 10 km/h
    {2.50f, 1.70f, 1.20f, 0.90f, 0.80f}, // 30 km/h
    {3.10f, 1.90f, 1.10f, 0.70f, 0.60f}, // 50 km/h
    {3.55f, 2.10f, 0.95f, 0.50f, 0.40f}, // 70 km/h
    {3.70f, 2.20f, 0.80f, 0.40f, 0.35f}  // 90 km/h
};

// Giving wind speed an index so we can chose a value from the table.
int wind_speed_index(WindSpeed speed) {
    return (int)speed;
}

// giving wind direction an index so we can choose a value from the table.
int dir_from_vec(int x, int y) {
    if (x == -1 && y == -1) {
        return 0; // wind blowing North west
    }
    if (x ==  0 && y == -1) {
        return 1; // wind blowing north
    }
    if (x ==  1 && y == -1){
        return 2; // wind blowing northeast
    }
    if (x == -1 && y ==  0) {
        return 3; // wind blowing west
    }
    if (x ==  1 && y ==  0) {
        return 4; // wind blowing east
    }
    if (x == -1 && y ==  1) {
        return 5; // wind blowing southwest
    }
    if (x ==  0 && y ==  1) {
        return 6; // wind blowing south
    }
    if (x ==  1 && y ==  1) {
        return 7; // wind blowing southeast
    }
    return -1;
}

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
            spreadToNeighbors(automaton, row, col);

        }

    }
}

void spreadToNeighbors(const CellularAutomaton* automaton, size_t row, size_t col) {
    // input validation
    assert(row < automaton->num_rows && "out of bounds");
    CellArray cells = automaton->rows[row];

    assert(col < cells.count && "out of bounds");
    Cell spreading_cell = cells.elements[col];

    // Looping over neighbouring cells
    for (int neighbour_row = (int)row - 1; neighbour_row <= (int)row + 1; neighbour_row++ ) {
        // if row is out of bounds = skip
        if (neighbour_row < 0 ) {
            continue;
        }
        if (neighbour_row >= (int)automaton->num_rows) {
            break;
        }
        CellArray neighbour_cell_arr = automaton->rows[neighbour_row];

        for (int neighbour_col = (int)col - 1; neighbour_col <= (int)col + 1; neighbour_col++) {
            // if column is out of bounds = skip
            if (neighbour_col < 0) {
                continue;
            }
            if (neighbour_col >= (int)cells.count) {
                break;
            }
            Cell* neighbouring_cell = &neighbour_cell_arr.elements[neighbour_col];

            if (neighbouring_cell->state != CELLSTATE_NORMAL) {
                continue;
            }
            // Calculate the position of the neighboring cell we are looking at.
            int dx = neighbour_col - (int)col;
            int dy = neighbour_row - (int)row;

            // Getting an index for what direction the wind is blowing (1,1) = 7 = southeast
            int wind_dir = dir_from_vec(automaton->windX, automaton->windY);
            int w_idx = wind_speed_index(automaton->speed);

            // finding out what direction the neighboring cell is in compared to the burning cell
            int neighbour_dir = dir_from_vec(dx, dy);

            // Calculating the difference in wind direction and the neighbor cell
            int diff = abs(wind_dir - neighbour_dir);
            if (diff > 4) diff = 8 - diff;

            int angle_index = diff;

            /* picking our wind factor value from the table, based on wind speed and direction
               compared to the cell burning cell
             */
            float a_w = wind_effect_table[w_idx][angle_index];

            // calculating the chance the spreading cell will ignite the neighbouring cell
            float chance = chanceToSpread(&spreading_cell, neighbouring_cell, a_w);
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

float chanceToSpread(const Cell* src, const Cell* dst, float a_w) {

    // tabel of nominal fire probability from source https://www.mdpi.com/2571-6255/3/3/26
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

    // Fine fuel moisture content, between 0 and 1, the higher the drier.
    float e_m = 1 - dst->moisture;

    // Slope angle set to 1, we will not implement this slope angle.
    constexpr float a_h = 1.0f;

    // wind and slope factor set to 1 for now
    float a_wh = a_w * a_h;

    // algorith from source, spread probability from burning cell to neighboring cell
    float p_burn = (1 - powf(1 - p_n,a_wh)) * e_m;

    return p_burn;
}

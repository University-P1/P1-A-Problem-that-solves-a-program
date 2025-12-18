#include "burnout_cell.h"

// We calculated that the timestep is roughly 2m 55s
// 1t = 2m + 55s

static const size_t burn_durations[VEG_LAST] = {
    1, // Broadleaves - 1t + 40s = 1 timestep (rounded down)
    1, // Shrubs 1t + 45sec = 1 timestep (rounded down)
    1, // Grassland 1t + 15-30sec (averaging to 25sec) = 1 timestep (rounded down)
    2, // Fireprone (pines) 1t + 167s = 2 timesteps (rounded down)
    11, // Agroforestry (trees and shrubs) 1t + not fireprone * 1/4 + shrubs * 3/4 = 10.5 + 1 = 11 timesteps (rounded down)
    42, // Not fireprone (trees (oak)) 1t + 2h = 42t (rounded down)
};

// The function that checks if a cell is burned out
static bool isBurnedOut(const Cell *cell)
{
    // If the cell is not burning, we can check if it is bruned out.
    // It is possible that the cell can be unburned or already burned out
    if (cell->state != CELLSTATE_ONFIRE) {
        return false; // returns false, because we only check active burning cells
    }

    size_t duration = burn_durations[vegTypeIndex(cell->type)];

    // Check if the time it burns exceeds the time for the given vegetation, to see if the cell is supposed to be "burned out"
    return cell->on_fire_counter >= duration;
}

CellularAutomaton burnoutCells(const CellularAutomaton* automaton) {
    CellularAutomaton res = cloneAutomaton(automaton);

    // Looping through all the rows
    for (size_t row = 0; row < res.num_rows; row++) {
        const CellArray arr = automaton->rows[row];
        const CellArray out_arr = res.rows[row];

            //Loop through all the collums in the rows
        for (size_t col = 0; col < arr.count; col++) {
            // get actual cell insted of the cloned version
            const Cell* cell = &arr.elements[col];
            Cell* out_cell = &out_arr.elements[col];

            // Check if the cell is burned out
            if (isBurnedOut(cell)) {
                out_cell->state = CELLSTATE_BURNT;

              // If the cell is burning apply the counter
            } else if (cell->state == CELLSTATE_ONFIRE) {
                out_cell->on_fire_counter++;
            }
        }
    }

    return res;
}

#include "burnout_cell.h"
#include "cell.h"

CellularAutomaton burnoutCells(const CellularAutomaton* automaton) {
    CellularAutomaton result = cloneAutomaton(automaton);

    return result;
}

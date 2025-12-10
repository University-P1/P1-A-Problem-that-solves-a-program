#pragma once
#include "cell.h"

/// Modifies the Cellular Automaton by spreading the fire between cells
CellularAutomaton directSpread(const CellularAutomaton* automaton);
int windDifferenceIndex(int ax, int ay, int bx, int by);

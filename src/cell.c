#include "cell.h"
#include <stdio.h>

const char* cellTypeToStr(VegType type) {
    switch (type) {
    case VEG_BROADLEAVES:
        return "BROAD LEAVES";
    case VEG_SHRUBS:
        return "SHRUBS";
    case VEG_GRASSLAND:
        return "GRASSLAND";
    case VEG_FIREPRONE:
        return "FIREPRONE";
    case VEG_AGROFORESTRY:
        return "AGRO FORESTRY";
    case VEG_NOTFIREPRONE:
        return "NOT FIREPRONE";

    default:
        fprintf(stderr, "Invalid CellState: %d\n", type);
        return "INVALID";
    }
}

const char* cellStateToStr(CellState state) {
    switch (state) {
    case CELLSTATE_NORMAL:
        return "NORMAL";
    case CELLSTATE_ONFIRE:
        return "ON FIRE";
    case CELLSTATE_BURNT:
        return "BURNT";

    default:
        fprintf(stderr, "Invalid CellState: %d\n", state);
        return "INVALID";
    }
}

void printCell(const Cell* cell, FILE* fd) {
    fprintf(fd, 
            "(Cell) {\n"
                "\t.moisture = %f\n"
                "\t.fuel = %f\n"
                "\t.heat = %f\n"
                "\t.state = %s\n"
                "\t.type = %s\n"
            "}\n",
            cell->moisture,
            cell->fuel,
            cell->heat,
            cellStateToStr(cell->state),
            cellTypeToStr(cell->type)
    );
}

void forEachCell(const CellularAutomaton* automaton, cellProc fn, void* userdata) {
    for(size_t row = 0; row < automaton->num_rows; row++) {
        const CellArray cell_row = automaton->rows[row];
        for(size_t col = 0; col < cell_row.count; col++) {
            fn(automaton, row, col, userdata);
        }
    }
}

static void printCellProc(const CellularAutomaton* automaton, size_t row, size_t col, void* userdata) {
    const Cell cell = automaton->rows[row].elements[col];
    FILE* file_out = userdata;
    printCell(&cell, file_out);
}

void printAutomaton(const CellularAutomaton* automaton, FILE *fd) {
    forEachCell(automaton, printCellProc, fd);
}

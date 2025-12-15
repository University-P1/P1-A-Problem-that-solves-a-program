#include "cell.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
                "\t.state = %s\n"
                "\t.type = %s\n"
            "}\n",
            cell->moisture,
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

CellularAutomaton cloneAutomaton(const CellularAutomaton* orig) {
    const int windX = orig->windX;
    const int windY = orig->windY;
    const WindSpeed speed = orig->speed;
    const size_t num_rows = orig->num_rows;
    assert(num_rows > 0 && "Automaton was empty");

    const size_t num_columns = orig->rows[0].count;

    const size_t cell_bytes = num_columns * num_rows * sizeof(Cell);
    CellArray* out_rows = malloc(num_rows * sizeof(CellArray));
    Cell* out_cells = malloc(cell_bytes);
    memcpy(out_cells, orig->rows[0].elements, cell_bytes);
    for (size_t row = 0; row < num_rows; row++) {
        Cell* row_cells = out_cells + row * num_columns;

        out_rows[row] = (CellArray) {
            .count = num_columns,
            .elements = row_cells,
        };
    }

    return (CellularAutomaton) {
        .windX = windX,
        .windY = windY,
        .speed = speed,
        .num_rows = num_rows,
        .rows = out_rows,
    };
}

void destroyAutomaton(const CellularAutomaton* automaton) {
    free(automaton->rows[0].elements);
    free(automaton->rows);
}

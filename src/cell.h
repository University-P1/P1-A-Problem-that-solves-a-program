#pragma once
#include <stdio.h>

typedef enum CellState {
    CELLSTATE_NORMAL,
    CELLSTATE_ONFIRE,
    CELLSTATE_BURNT,
} CellState;
const char* cellStateToStr(CellState state);


typedef enum VegType {
    VEG_BROADLEAVES,
    VEG_SHRUBS,
    VEG_GRASSLAND,
    VEG_FIREPRONE,
    VEG_AGROFORESTRY,
    VEG_NOTFIREPRONE,

    VEG_LAST,
} VegType;
const char* cellTypeToStr(VegType type);

typedef struct Cell {
    float moisture;
    float fuel;
    float heat;
    VegType type;
    CellState state;
    //CellType type;
} Cell;
void printCell(const Cell* cell, FILE* fd);

typedef struct CellArray {
    Cell* elements;
    size_t count;
} CellArray;

typedef struct CellularAutomaton {
    CellArray* const rows;
    size_t num_rows;
    int windX;
    int windY;
    float wind_speed;
    /* other stuff maybe */
} CellularAutomaton;
void printAutomaton(const CellularAutomaton* automaton, FILE* fd);

typedef void (*cellProc)(const CellularAutomaton* automaton, size_t row, size_t col, void* userdata);
void forEachCell(const CellularAutomaton* automaton, cellProc fn, void* userdata);

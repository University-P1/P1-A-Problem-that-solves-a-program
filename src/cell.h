#pragma once
#include <stdio.h>

typedef enum CellState {
    CELLSTATE_NORMAL = 'N',
    CELLSTATE_ONFIRE = 'F',
    CELLSTATE_BURNT = 'O',
} CellState;
const char* cellStateToStr(CellState state);


typedef enum VegType {
    VEG_BROADLEAVES = 'L',
    VEG_SHRUBS = 'S',
    VEG_GRASSLAND = 'G',
    VEG_FIREPRONE = 'F',
    VEG_AGROFORESTRY = 'A',
    VEG_NOTFIREPRONE = 'N',

    VEG_LAST = 6,
} VegType;
const char* cellTypeToStr(VegType type);
inline size_t vegTypeIndex(VegType type) {
    switch (type) {
    case VEG_BROADLEAVES:
        return 0;
    case VEG_SHRUBS:
        return 1;
    case VEG_GRASSLAND:
        return 2;
    case VEG_FIREPRONE:
        return 3;
    case VEG_AGROFORESTRY:
        return 4;
    case VEG_NOTFIREPRONE:
        return 5;
    default:
        return 0;
    }
}

typedef enum WindSpeed {
    WIND_NONE,
    WIND_SLOW,
    WIND_MODERATE,
    WIND_FAST,
    WIND_EXTREME,

    WIND_LAST,
} WindSpeed;

typedef struct Cell {
    float moisture;
    size_t on_fire_counter;
    VegType type;
    CellState state;
} Cell;
void printCell(const Cell* cell, FILE* fd);

typedef struct CellArray {
    Cell* elements;
    size_t count;
} CellArray;

typedef struct CellularAutomaton {
    CellArray* rows;
    size_t num_rows;
    int windX;
    int windY;
    WindSpeed speed;
    /* other stuff maybe */
} CellularAutomaton;
void printAutomaton(const CellularAutomaton* automaton, FILE* fd);

typedef void (*cellProc)(const CellularAutomaton* automaton, size_t row, size_t col, void* userdata);
void forEachCell(const CellularAutomaton* automaton, cellProc fn, void* userdata);

CellularAutomaton cloneAutomaton(const CellularAutomaton* automaton);

#include "input.h"
#include "cell.h"

char* findChar(char c);

/// Parse an integer, this function looks a max of 11 bytes ahead
/// The maximum integer number is 10 digits, and a possible negation sign also takes 1 digit.
/// @return Returns the number of bytes read
uint8_t parseNumber(const char* buf, size_t max_len, int* out);

typedef enum ParseState {
    PARSE_STATE_HEADER_WIDTH,
    PARSE_STATE_HEADER_HEIGHT,
    PARSE_STATE_HEADER_WINDX,
    PARSE_STATE_HEADER_WINDY,
    PARSE_STATE_CELLS,
} ParseState;

CellularAutomaton readInitialState(const char* path) {
    CellularAutomaton automaton = {
        .rows = nullptr,
        .num_rows = 0,
        .windX = 0,
        .windY = 0
    };

    FILE* fd = fopen(path, "r");
    if (fd == nullptr) goto _err;


    ParseState state = PARSE_STATE_HEADER_WIDTH;
    int width, height;
    float windX, windY;
    char buf[256];
    do {
        size_t num_bytes = fread(buf, 1, 256, fd);
        if (num_bytes == 0) continue;

        size_t current_pos = 0;

parse_next_value:
        switch (state) {
            case PARSE_STATE_HEADER_WIDTH: {
                size_t bytes_left = num_bytes - current_pos;
                current_pos += parseNumber(buf + current_pos, bytes_left, &width);

                // The next step
                state = PARSE_STATE_HEADER_HEIGHT;

                // There are more bytes remaining
                if (current_pos < num_bytes) goto parse_next_value;

                // No more bytes :( try to read more
                } break;

            case PARSE_STATE_HEADER_HEIGHT: {
                size_t bytes_left = num_bytes - current_pos;
                current_pos += parseNumber(buf + current_pos, bytes_left, &height);

                // The next step
                state = PARSE_STATE_HEADER_WINDY;

                // There are more bytes remaining
                if (current_pos < num_bytes) goto parse_next_value;

                // No more bytes :( try to read more
                } break;

            case PARSE_STATE_HEADER_WINDX:
                break;
            case PARSE_STATE_HEADER_WINDY:
                break;
            case PARSE_STATE_CELLS:
                break;
        }

    } while(feof(fd) || ferror(fd));


_err:
    if (fd) fclose(fd);
    return (CellularAutomaton) {
        .rows = nullptr,
        .num_rows = 0,
        .windX = 0,
        .windY = 0
    };
}

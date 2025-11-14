cmake_minimum_required(VERSION 4.0.2)
project(wildire-spotting C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(wildfire-spotting
    src/main.c
    src/direct_spread.c
    src/spotting_spread.c
    src/display.c
    src/input.c
    src/burnout_cell.c
)

set_target_properties(wildfire-spotting PROPERTIES
    C_STANDARD 23
    C_EXTENSIONS OFF
)

target_compile_options(wildfire-spotting PRIVATE 
    # Enable all warnings
    -Wall
    -Wextra
    -Wpedantic
    -Wnarrowing # useful for overflow errors
    -Wconversion
)

install(TARGETS wildfire-spotting DESTINATION bin)

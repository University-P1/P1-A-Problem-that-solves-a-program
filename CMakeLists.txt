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

set_property(TARGET wildfire-spotting PROPERTY C_STANDARD 23)
set_property(TARGET wildfire-spotting PROPERTY C_EXTENSIONS NO)

install(TARGETS wildfire-spotting DESTINATION bin)

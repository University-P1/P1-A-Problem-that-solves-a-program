cmake_minimum_required(VERSION 4.0.2)
project(wildire-spotting C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# set the output directory for built objects.
# This makes sure that the dynamic library goes into the build directory automatically.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/")

set(SDL_X11 OFF)
set(SDL_WAYLAND ON)

# SDL
add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)

add_executable(wildfire-spotting
    src/main.c
    src/cell.c
    src/direct_spread.c
    src/spotting_spread.c
    src/display.c
    src/input.c
    src/burnout_cell.c
)

# Link to the actual SDL3 library.
target_link_libraries(wildfire-spotting PRIVATE SDL3::SDL3 m)

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
    -Wdeprecated
    # -Werror damn SDL is riddled with warnings
)

# if(CMAKE_BUILD_TYPE STREQUAL "Debug")
#     target_compile_options(wildfire-spotting PRIVATE
#         -fsanitize=address
#     )
# endif()

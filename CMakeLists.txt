cmake_minimum_required(VERSION 4.0.2)
project(wildire-spotting C)

set(CMAKE_C_STANDARD 17)

# Source files
file(GLOB_RECURSE SOURCES
    RELATIVE ${PROJECT_SOURCE_DIR}
    src/*.c
)

add_executable(wildfire-spotting ${SOURCES})

install(TARGETS wildfire-spotting DESTINATION bin)

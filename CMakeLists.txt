cmake_minimum_required(VERSION 3.16)
project(LedgeLite LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Include directory
include_directories(include)

# Detect dependencies
find_package(SQLite3 REQUIRED)

# Source files
file(GLOB_RECURSE SOURCES src/*.cxx)

# Define the executable
add_executable(LedgeLite ${SOURCES})

# Link dependency libraries
target_include_directories(LedgeLite PRIVATE ${SQLite3_INCLUDE_DIRS})
target_link_libraries(LedgeLite PRIVATE ${SQLite3_LIBRARIES})

# Optional: add warnings
target_compile_options(LedgeLite PRIVATE -Wall -Wextra -O2)

# Include Debug Symbols
set(CMAKE_BUILD_TYPE Debug)

cmake_minimum_required(VERSION 3.12)

project(TinyFileDialogs)

# Create static library from tinyfiledialogs source
add_library(tinyfiledialogs STATIC
    tinyfiledialogs.c
)

# Include directories
target_include_directories(tinyfiledialogs PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# No external dependencies for tinyfiledialogs

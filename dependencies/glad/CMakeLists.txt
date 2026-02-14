cmake_minimum_required(VERSION 3.12)

project(Glad)

# Create static library from glad source
add_library(glad STATIC
  src/gl.c
)

# Include directories
target_include_directories(glad PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)
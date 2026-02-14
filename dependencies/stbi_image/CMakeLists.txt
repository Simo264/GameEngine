cmake_minimum_required(VERSION 3.12)

project(STBImage)

# Create static library from stb_image source
add_library(stbi_image STATIC
  stb_image.c
)

# Include directories
target_include_directories(stbi_image PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)
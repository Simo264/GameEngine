cmake_minimum_required (VERSION 3.12)

project(GameEngine)

if(MSVC)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:MSVCRT")
endif()

set(COMMON_CXX_FLAGS "${COMMON_CXX_FLAGS} -Wall -pedantic -Wextra -m64 -mavx2 -mfma -ffast-math")
set(GLFW_BUILD_DOCS      OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS     OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES  OFF CACHE BOOL "" FORCE)

# Add include directory path
set(EXT_INCLUDE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/Externals/Include/)  # externals include path directory
set(INT_INCLUDE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/Source/)             # internal include path directory
include_directories(${EXT_INCLUDE_PATH})
include_directories(${INT_INCLUDE_PATH})

# List external source files
file(GLOB_RECURSE EXTERNAL_IMGUI_SOURCES
  ${EXT_INCLUDE_PATH}/imgui/*
)
set(EXTERNAL_OPENGL_SOURCE 
  ${EXT_INCLUDE_PATH}/opengl/gl.c 
  ${EXT_INCLUDE_PATH}/opengl/gl.h
  ${EXT_INCLUDE_PATH}/opengl/khrplatform.h 
)
set(EXTERNAL_TINYFD_SOURCE 
  ${EXT_INCLUDE_PATH}/tinyfiledialogs/tinyfiledialogs.h
  ${EXT_INCLUDE_PATH}/tinyfiledialogs/tinyfiledialogs.c
) 
set(EXTERNAL_SOURCES
  ${EXTERNAL_IMGUI_SOURCES}
  ${EXTERNAL_OPENGL_SOURCE}
  ${EXTERNAL_TINYFD_SOURCE}
)
# Group externals in filters
source_group("Externals\\imgui"           FILES ${EXTERNAL_IMGUI_SOURCES})  
source_group("Externals\\opengl"          FILES ${EXTERNAL_OPENGL_SOURCE})  
source_group("Externals\\tinyfiledialogs" FILES ${EXTERNAL_TINYFD_SOURCE})

# List all Engine files
file(GLOB_RECURSE ENGINE_SOURCES 
  ${CMAKE_CURRENT_SOURCE_DIR}/Source/**.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Source/**.hpp
)

# Group them in filters
foreach(_source IN ITEMS ${ENGINE_SOURCES})
  get_filename_component(_source_path "${_source}" PATH)
  string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" _group_path "${_source_path}")
  string(REPLACE "/" "\\" _group_path "${_group_path}")
  source_group("${_group_path}" FILES "${_source}")
endforeach()

# Link all files into executable
add_executable (GameEngine 
  ${ENGINE_SOURCES}
  ${EXTERNAL_SOURCES}
)

# Linking
target_link_libraries(GameEngine ${CMAKE_CURRENT_SOURCE_DIR}/Externals/Libs/glfw3.lib) 
target_link_libraries(GameEngine ${CMAKE_CURRENT_SOURCE_DIR}/Externals/Libs/spdlogd.lib)
target_link_libraries(GameEngine ${CMAKE_CURRENT_SOURCE_DIR}/Externals/Libs/assimp.lib)

# If the build is Debug, use yaml-cppd.lib; otherwise, use yaml-cpp.lib
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  # cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
  target_link_libraries(GameEngine ${CMAKE_CURRENT_SOURCE_DIR}/Externals/Libs/yaml-cppd.lib)
else()
  # cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
  target_link_libraries(GameEngine ${CMAKE_CURRENT_SOURCE_DIR}/Externals/Libs/yaml-cpp.lib)
endif()

target_compile_features(GameEngine PRIVATE cxx_std_20)
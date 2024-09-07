cmake_minimum_required (VERSION 3.8)

# Enable fast reload for MSVC compilers, if supported
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("GameEngine")

set(COMMON_CXX_FLAGS "${COMMON_CXX_FLAGS} -Wall -pedantic -Wextra -m64 -mavx2 -mfma -ffast-math")
set( GLFW_BUILD_DOCS      OFF CACHE BOOL "" FORCE )
set( GLFW_BUILD_TESTS     OFF CACHE BOOL "" FORCE )
set( GLFW_BUILD_EXAMPLES  OFF CACHE BOOL "" FORCE )


# Add include directory path
set(EXT_INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/Externals/Include/")  # externals include path directory
set(INT_INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/Source/")             # internal include path directory
include_directories(${EXT_INCLUDE_PATH})
include_directories(${INT_INCLUDE_PATH})


# List external source files
file(GLOB_RECURSE EXTERNAL_IMGUI_SOURCES
  ${EXT_INCLUDE_PATH}/imgui/*
)
set(EXTERNAL_GLAD_SOURCE 
  ${EXT_INCLUDE_PATH}/Glad/gl.c 
)
set(EXTERNAL_TINYFD_SOURCE 
  ${EXT_INCLUDE_PATH}/tinyfiledialogs/tinyfiledialogs.c
) 
set(EXTERNAL_SOURCES
  ${EXTERNAL_IMGUI_SOURCES}
  ${EXTERNAL_GLAD_SOURCE}
  ${EXTERNAL_TINYFD_SOURCE}
)
# Group externals in filters
source_group("Externals\\imgui"           FILES ${EXTERNAL_IMGUI_SOURCES})  
source_group("Externals\\glad"            FILES ${EXTERNAL_GLAD_SOURCE})  
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

# Static linking
target_link_libraries(GameEngine "${CMAKE_CURRENT_SOURCE_DIR}/Externals/Libs/glfw3.lib") 
target_link_libraries(GameEngine "${CMAKE_CURRENT_SOURCE_DIR}/Externals/Libs/spdlogd.lib")
target_link_libraries(GameEngine "${CMAKE_CURRENT_SOURCE_DIR}/Externals/Libs/assimp.lib")
target_link_libraries(GameEngine "${CMAKE_CURRENT_SOURCE_DIR}/Externals/Libs/freetype.lib")


if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET GameEngine PROPERTY CXX_STANDARD 20)
endif()

if(MSVC)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:MSVCRT")
endif()

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
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/Externals/Include")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/Source/")


# Separate all .cpp .hpp files in different VS filters
# List all .cpp and .hpp files into SOURCES variabile
FILE(GLOB_RECURSE SOURCES 
  ${CMAKE_CURRENT_SOURCE_DIR}/Source/**.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Source/**.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Source/**.c
  ${CMAKE_CURRENT_SOURCE_DIR}/Source/**.h
)

# Add source files
add_executable (GameEngine 
  ${SOURCES}
)

# Linking libraries
target_link_libraries(GameEngine "${CMAKE_CURRENT_SOURCE_DIR}/Externals/Libs/glfw3.lib") 
target_link_libraries(GameEngine "${CMAKE_CURRENT_SOURCE_DIR}/Externals/Libs/spdlogd.lib")
target_link_libraries(GameEngine "${CMAKE_CURRENT_SOURCE_DIR}/Externals/Libs/assimp.lib")


if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET GameEngine PROPERTY CXX_STANDARD 20)
endif()

if(MSVC)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:MSVCRT")
endif()

cmake_minimum_required (VERSION 3.12)

project(GameEngine)

if(MSVC)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:MSVCRT")
endif()

include(FetchContent)
set(FETCHCONTENT_BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies)
set(FETCHCONTENT_QUIET FALSE)

# Download and make glad dependency available in the project.
FetchContent_Declare(
  glad
  GIT_REPOSITORY https://github.com/Dav1dde/glad
  GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(glad)


# Download and make glfw dependency available in the project.
set(GLFW_BUILD_DOCS      OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS     OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES  OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
  glfw
  GIT_REPOSITORY  https://github.com/glfw/glfw.git
  GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(glfw)

# Download and make glm dependency available in the project.
FetchContent_Declare(
  glm
  GIT_REPOSITORY  https://github.com/g-truc/glm.git
  GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(glm)

# Download and make entt dependency available in the project.
FetchContent_Declare(
  entt
  GIT_REPOSITORY  https://github.com/skypjack/entt.git
  GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(entt)

# Download and make nlohmann-json dependency available in the project.
set(BUILD_SHARED_LIBS ON) # Enable build shared libs
FetchContent_Declare(
  json 
  URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
)
FetchContent_MakeAvailable(json)
set(BUILD_SHARED_LIBS OFF) # Disable build shared libs

# Download and make yaml-cpp dependency available in the project.
set(BUILD_SHARED_LIBS ON) # Enable build shared libs
FetchContent_Declare(
  yaml-cpp
  GIT_REPOSITORY  https://github.com/jbeder/yaml-cpp.git
  GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(yaml-cpp)
set(BUILD_SHARED_LIBS OFF) # Disable build shared libs

# Download and make assimp dependency available in the project.
set(BUILD_SHARED_LIBS ON) # Enable build shared libs
FetchContent_Declare(
  assimp
  GIT_REPOSITORY  https://github.com/assimp/assimp.git
  GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(assimp)
set(BUILD_SHARED_LIBS OFF) # Disable build shared libs

# tinyfiledialogs configuration
include_directories(Dependencies/tinyfiledialogs)

# stbi_image include path directory
include_directories(Dependencies/stbi_image)

# spdlog configuration
add_subdirectory(Dependencies/spdlog)

# imgui configuration
add_subdirectory(Dependencies/imgui)



# Add include directory path
include_directories(Source/)

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
  #Dependencies/glad/gl.c
  Dependencies/tinyfiledialogs/tinyfiledialogs.c
  ${ENGINE_SOURCES}
)

# Copy assimp, yaml-cpp dlls into the executable directory
add_custom_command(TARGET GameEngine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:assimp> $<TARGET_FILE_DIR:GameEngine>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:yaml-cpp> $<TARGET_FILE_DIR:GameEngine>
)

# Linking
target_link_libraries(GameEngine 
  glad  
  glfw
  glm::glm
  spdlog
  imgui
  assimp
  EnTT::EnTT
  yaml-cpp::yaml-cpp
  nlohmann_json::nlohmann_json
)

target_compile_features(GameEngine PRIVATE cxx_std_20)

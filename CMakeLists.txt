cmake_minimum_required (VERSION 3.12)
cmake_policy(SET CMP0135 NEW)
cmake_policy(SET CMP0079 NEW)

project(proto_engine)

include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)

# GLFW configuration info
if(NOT DEFINED GLFW_BUILD_WAYLAND)
  set(GLFW_BUILD_WAYLAND ON CACHE BOOL "Build GLFW with Wayland support")
endif()
if(NOT DEFINED GLFW_BUILD_X11)
  set(GLFW_BUILD_X11 ON CACHE BOOL "Build GLFW with X11 support")
endif()

message(STATUS "GLFW configuration:")
message(STATUS "  GLFW_BUILD_WAYLAND: ${GLFW_BUILD_WAYLAND}")
message(STATUS "  GLFW_BUILD_X11: ${GLFW_BUILD_X11}")

# Download and make glfw dependency available in the project.
set(GLFW_BUILD_DOCS      OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS     OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES  OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_WAYLAND ${GLFW_BUILD_WAYLAND} CACHE BOOL "" FORCE)
set(GLFW_BUILD_X11 ${GLFW_BUILD_X11} CACHE BOOL "" FORCE)
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
  GIT_TAG main
  GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(entt)

# Download and make spdlog dependency available in the project.
FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG v1.x
  GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(spdlog)

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

# glad configuration
add_subdirectory(dependencies/glad/)
# stbi_image configuration
add_subdirectory(dependencies/stbi_image/)
# tinyfiledialogs configuration
add_subdirectory(dependencies/tinyfiledialogs/)
# imgui configuration
add_subdirectory(dependencies/imgui/)
# Link GLFW to imgui target (needed for imgui_impl_glfw.cpp)
target_link_libraries(imgui PUBLIC glfw)
# include src/
include_directories(src/)

# List all Engine files
file(GLOB_RECURSE engine_src
  ${CMAKE_CURRENT_SOURCE_DIR}/src/**.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/**.hpp
)

# Link all files into executable
add_executable (${PROJECT_NAME}
  ${engine_src}
)

# Copy assimp, yaml-cpp dlls into the executable directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:assimp> $<TARGET_FILE_DIR:${PROJECT_NAME}>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:yaml-cpp> $<TARGET_FILE_DIR:${PROJECT_NAME}>
)

# Linking
target_link_libraries(${PROJECT_NAME}
  assimp
  imgui
  glad
  stbi_image
  tinyfiledialogs
  glfw
  spdlog::spdlog
  glm::glm
  EnTT::EnTT
  yaml-cpp::yaml-cpp
  nlohmann_json::nlohmann_json
)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(${PROJECT_NAME} PRIVATE
    -Wextra
    -Wconversion
    -Wunused
  )
endif()

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

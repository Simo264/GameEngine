cmake_minimum_required (VERSION 3.12)

project(Engine)

# List all Engine files
file(GLOB_RECURSE engine_src 
  ${CMAKE_CURRENT_SOURCE_DIR}/**.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/**.hpp
)

# Group them in filters
foreach(_source IN ITEMS ${engine_src})
  get_filename_component(_source_path "${_source}" PATH)
  string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" _group_path "${_source_path}")
  string(REPLACE "/" "\\" _group_path "${_group_path}")
  source_group("${_group_path}" FILES "${_source}")
endforeach()

add_library(Engine STATIC
  ${engine_src}
)

target_include_directories(Engine PUBLIC 
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(Engine
  assimp
  imgui
  glfw
  spdlog::spdlog
  glm::glm
  EnTT::EnTT
  yaml-cpp::yaml-cpp
  nlohmann_json::nlohmann_json
)

target_compile_options(Engine PRIVATE 
  -Wextra  
  -Wconversion
  -Wunused
  # -Wshadow
  # -Wold-style-cast
  # -Wpedantic 
  # -Wpointer-arith -Wcast-qual -Wno-missing-braces
)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(Engine PRIVATE 
    -Wextra  
    -Wconversion
    -Wunused
    #-Wshadow
    #-Wold-style-cast
    #-Wpedantic 
    #-Wpointer-arith 
    #-Wcast-qual 
    #-Wno-missing-braces
  )
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  target_compile_options(Engine PRIVATE
    /W4
    /permissive-
    /sdl
  )
endif()

target_compile_features(Engine PRIVATE cxx_std_20)
cmake_minimum_required (VERSION 3.12)

project(Imgui)

add_library(imgui STATIC 
  imgui.cpp
  imgui_draw.cpp
  imgui_demo.cpp
  imgui_tables.cpp
  imgui_widgets.cpp
  ImGuizmo.cpp
  backends/imgui_impl_opengl3.cpp
  backends/imgui_impl_glfw.cpp
)

# Includi le directory di intestazione
target_include_directories(imgui PUBLIC 
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/backends
)

target_link_libraries(imgui PRIVATE glfw)
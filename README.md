# ProtoEngine

A modern C++ game engine with ECS architecture, OpenGL rendering, and ImGui-based editor.

## Features

- Entity Component System (ECS) using EnTT
- OpenGL 4.6 rendering with GLAD
- GLFW for window and input management
- ImGui for editor interface
- Asset loading (images via STB Image, models via Assimp)
- YAML-based scene serialization
- JSON configuration support
- Cross-platform file dialogs (TinyFileDialogs)

## Dependencies

The project uses CMake's FetchContent to automatically download and build:
- GLFW (window management)
- GLM (mathematics)
- EnTT (ECS)
- spdlog (logging)
- nlohmann/json (JSON parsing)
- yaml-cpp (YAML parsing)
- Assimp (3D model loading)

Static libraries built from source:
- GLAD (OpenGL loading)
- STB Image (image loading)
- TinyFileDialogs (file dialogs)
- ImGui (GUI framework)

## Building

### Prerequisites

- CMake 3.12 or higher
- C++20 compatible compiler
- OpenGL development libraries
- Wayland or X11 development libraries (for GLFW)

#### Linux Package Dependencies

**Fedora/RHEL-based:**
```bash
sudo dnf install mesa-libGL-devel
```

**Debian/Ubuntu-based:**
```bash
sudo apt-get install libgl1-mesa-dev
```

### Linux Build Instructions

#### With Wayland (Recommended on modern Linux distributions)

```bash
# Create build directory
mkdir -p build
cd build

# Configure with Wayland support (disables X11)
cmake -DGLFW_BUILD_WAYLAND=ON -DGLFW_BUILD_X11=OFF ..

# Build
cmake --build . --parallel $(nproc)

# Run
./proto_engine
```

#### With X11 (Traditional Linux desktop)

```bash
# Create build directory
mkdir -p build
cd build

# Configure with X11 support (requires X11 development packages)
cmake -DGLFW_BUILD_WAYLAND=OFF -DGLFW_BUILD_X11=ON ..

# Build
cmake --build . --parallel $(nproc)

# Run
./proto_engine
```

#### Default configuration (automatic detection)

```bash
# Create build directory
mkdir -p build
cd build

# Let CMake auto-detect available window system
cmake ..

# Build
cmake --build . --parallel $(nproc)

# Run
./proto_engine
```

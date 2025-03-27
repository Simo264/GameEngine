# How to build (Linux + GNU Makefiles)
1. ```cmake -G "Unix Makefiles" -S . -B Build/```
2. ```make -C Build/```
3. ```./Build/GameEngine```

# How to build (Windows + Visual Studio 22)
1. ```cmake -G "Visual Studio 17 2022" -A x64 -S . -B Build/```
2. Open Visual Studio solution file (.sln)
3. Compile solution

<!-- I am developing this project in a windows environment using 
Visual Studio Community 2022 as IDE and MSVC as C++ compiler

# Build project
- cd ./GameEngine
- mkdir Build
- cmake -S . -B Build

### Current features following https://learnopengl.com/
- Lighting
    + [X] Colors
    + [X] Basic lighting
    + [X] Materials
    + [X] Lighting maps
    + [X] Light casters
    + [X] Multiple lights
- [X] Model loader with Assimp
- Advanced OpenGL
    + [X] Depth testin
    + [X] Stencil testing
    + [X] Blending
    + [X] Face culling
    + [X] Framebuffers
    + [X] Cubemaps
    + [X] Advanced data
    + [X] Advanced GLSL
    + [X] Geometry Shader
    + [X] Instancing
    + [X] Anti aliasing
- Advanced lighting
    + [X] Advanced lighting
    + Shadows
        - [X] Shadow mapping
        - [X] Point Shadows
    + [X] Normal mapping
    + [X] Parallax mapping
    + [ ] HDR
    + [ ] Bloom
    + [ ] Deferred Shading
    + [ ] SSAO
- PBR
    + [ ] Theory
    + [ ] Lighting
    + IBL
        + [ ] Diffuse irradiance
        + [ ] Specular IBL
- [ ] Text rendering
- Order-independent transparency (OIT)
    + [ ] Introduction
    + [ ] Weighted Blended
- [ ] Skeletal Animation
- [ ] Cascaded Shadow Mapping
- Scene
    + [ ] Scene Graph
    + [ ] Frustum Culling
- Tessellation
    + [x] Height Maps
    + [x] Tessellation
- [ ] Compute shaders
- [ ] Physically Based Bloom
- [ ] Area Lights -->

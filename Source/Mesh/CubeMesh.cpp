#include "CubeMesh.hh"



CubeMesh::CubeMesh() : Mesh(), Actor()
{
  // default vertices and indices
  // ---------------------------------
#if 0
  float vertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
  };
#endif
  float vertices[] = {
     1.0f,  1.0f, 0.0f,  1.0f, 1.0f, // top right front
     1.0f, -1.0f, 0.0f,  1.0f, 0.0f, // bottom right front
    -1.0f, -1.0f, 0.0f,  0.0f, 0.0f, // bottom left front
    -1.0f,  1.0f, 0.0f,  0.0f, 1.0f, // top left front

     1.0f,  1.0f, -1.0f, 1.0f, 1.0f, // top right back
     1.0f, -1.0f, -1.0f, 1.0f, 0.0f, // bottom right back
    -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, // bottom left back
    -1.0f,  1.0f, -1.0f, 0.0f, 1.0f, // top left back
  };
  uint32_t indices[] = {  // note that we start from 0!
    // front
    0,1,3,
    1,2,3,
    // back
    4,5,6,
    6,7,4,
    // right
    0,1,5,
    4,5,0,
    // left
    2,3,6,
    6,3,7,
    // top
    0,3,4,
    3,4,7,
    // bottom
    1,2,5,
    1,5,6
  };


  // default configuration
  // ---------------------------------
  Graphics::VAConfiguration config;
  config.PushAttribute(3); // vec3 position   attribute per vertex
  config.PushAttribute(2); // vec2 textCoords attribute per vertex


  Init(sizeof(vertices), vertices, sizeof(indices), indices, config);
}

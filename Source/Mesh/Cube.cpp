#include "Cube.hh"
#include "../Graphics/Renderer.hh"

Cube::Cube()
{
  static float    vertices[] = {
     1.0f,  1.0f, 0.0f,  // top right front
     1.0f, -1.0f, 0.0f,  // bottom right front
    -1.0f, -1.0f, 0.0f,  // bottom left front
    -1.0f,  1.0f, 0.0f,  // top left front

     1.0f,  1.0f, -1.0f,  // top right back
     1.0f, -1.0f, -1.0f,  // bottom right back
    -1.0f, -1.0f, -1.0f,  // bottom left back
    -1.0f,  1.0f, -1.0f   // top left back
  };
  static uint32_t indices[]  = {  // note that we start from 0!
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

  Graphics::VAConfiguration config;
  config.PushAttribute(3); // vec3 position attribute per vertex
  vertexArray.Init(sizeof(vertices), vertices, sizeof(indices), indices, config);

  glBindVertexArray(vertexArray.vao);
  
  glGenBuffers(1, &instanceBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, instanceBuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(mat4f) * 10, nullptr, GL_DYNAMIC_DRAW);

  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(mat4f), reinterpret_cast<void*>(0 * sizeof(vec4f)));
  glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(mat4f), reinterpret_cast<void*>(1 * sizeof(vec4f)));
  glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(mat4f), reinterpret_cast<void*>(2 * sizeof(vec4f)));
  glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(mat4f), reinterpret_cast<void*>(3 * sizeof(vec4f)));
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);
  glEnableVertexAttribArray(3);
  glEnableVertexAttribArray(4);
  glVertexAttribDivisor(1, 1);
  glVertexAttribDivisor(2, 1);
  glVertexAttribDivisor(3, 1);
  glVertexAttribDivisor(4, 1);

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  position = vec3f(0.0f, 0.0f, 0.0f);
  scaling  = vec3f(1.0f, 1.0f, 2.0f);
  rotation = vec3f(1.0f, 1.0f, 1.0f);
  rotationAngle = 0.0f;
}

void Cube::Draw(Graphics::Shader& shader)
{
  _model = glm::scale(mat4f(1.0f), scaling) * 
           glm::rotate(mat4f(1.0f), glm::radians(rotationAngle), rotation) * 
           glm::translate(mat4f(1.0f), position);

  shader.SetMat4f("model", _model);
  Graphics::Renderer::DrawIndexed(vertexArray);
}

void Cube::Destroy()
{
  vertexArray.Destroy();
}
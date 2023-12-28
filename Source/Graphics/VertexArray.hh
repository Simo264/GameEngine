#pragma once

#include "../Core.hh"


class VAConfiguration
{
public:
  VAConfiguration();

  void PushAttribute(uint8_t attribute);

  // ex: layout = [3, 3, 2]
  // layout=0: in vec3 components (x,y,z)
  // layout=1: in vec3 components (x,y,z)
  // layout=2: in vec2 components (x,y)
  array<uint8_t, 16> layout;
  uint32_t numAttrs;
};

class VertexArray
{
public:
  VertexArray() : _ebo{ 0 }, numVertices{ 0 }, numIndices{ 0 } {}
  ~VertexArray() = default;

  VertexArray(const VertexArray&) = delete;            // delete copy constructor
  VertexArray& operator=(const VertexArray&) = delete; // delete assign op

  void Create(uint32_t vertSize, float* vertData, uint32_t indSize, uint32_t* indData, VAConfiguration& config);
  void Destroy();

  void Bind()   const { glBindVertexArray(_vao); };
  void Unbind() const { glBindVertexArray(0); };

  uint32_t numVertices;
  uint32_t numIndices;

private:
  uint32_t _vao; 
  uint32_t _vbo;
  uint32_t _ebo;
};

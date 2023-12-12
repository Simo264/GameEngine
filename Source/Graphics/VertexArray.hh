#pragma once

#include "../Core.hh"


namespace Graphics
{
  class VAConfiguration
  {
  public:
    VAConfiguration();

    void PushAttribute(uint32_t attribute);

    // ex: layout = [3, 3, 2]
    // layout=0: in vec3 components (x,y,z)
    // layout=1: in vec3 components (x,y,z)
    // layout=2: in vec2 components (x,y)
    array<uint32_t, 5> layout;
    uint32_t numAttrs;
  };


	class VertexArray
	{
  public:
    VertexArray();
    ~VertexArray() = default;

    VertexArray(const VertexArray&) = delete;            // delete copy constructor
    VertexArray& operator=(const VertexArray&) = delete; // delete assign op

    void Init(uint32_t vertSize, float* vertData, VAConfiguration& config);
    void Init(uint32_t vertSize, float* vertData, uint32_t indSize, uint32_t* indData, VAConfiguration& config);

    void Destroy();

    uint32_t vao; 
    uint32_t vbo, numVertices;
    uint32_t ebo, numIndices;
	};
}

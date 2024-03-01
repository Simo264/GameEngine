#pragma once

#include "Core/Core.hpp"

/* ------------------------------------ 
  Vertex buffer layout 

  e.g: from vertex data stream
    {0,0,1, 0,0,0, 1,0} -> vec3, vec3, vec2
    {0,0,1, 0,0,0, 0,1} -> vec3, vec3, vec2
    {...}               -> vec3, vec3, vec2
  
  we describe that stream of data as: attributes = [3, 3, 2]
    layout=0: in vec3 components (x,y,z)
    layout=1: in vec3 components (x,y,z)
    layout=2: in vec2 components (x,y)
  ------------------------------------ */
class VertexBufferLayout
{
public:
  VertexBufferLayout()
  {
    numAttrs = 0;
    attributes.fill(0);
  }
  
  void PushAttribute(uint8_t attribute)
  {
    attributes[numAttrs++] = attribute;
  }
  
  void PushAttributes(std::initializer_list<uint8_t> values) 
  {
    for (int i = numAttrs; i < values.size(); i++, numAttrs++)
      attributes[i] = *(values.begin() + i);
  }
  Array<uint8_t, 16> attributes;
  uint32_t numAttrs;
};

/* --------------------------------------------------------------
  pack the vertex buffer data that must be sent to the GPU into 
  the struct
  -------------------------------------------------------------- */
struct VertexBufferData
{
  uint64_t	vertexDataSize; /* size in bytes */
  float*    vertextDataPtr;
  uint64_t	indexDataSize;  /* size in bytes */
  uint32_t* indexDataPtr;   

  VertexBufferData() : 
    vertexDataSize{ 0 }, 
    vertextDataPtr{ nullptr }, 
    indexDataSize{ 0 }, 
    indexDataPtr{ nullptr } 
  {}
  VertexBufferData(uint64_t vSize, float* vData, uint64_t iSize, uint32_t* iData) : 
    vertexDataSize{ vSize }, 
    vertextDataPtr{ vData }, 
    indexDataSize{ iSize }, 
    indexDataPtr{ iData } 
  {}
};


/* ------------------------------------
      Vertex array class
  ------------------------------------ */
class VertexArray
{
public:
  /* Generates vertex array, vertex buffer and index buffer objects */
  VertexArray();

  /* Disable copy constructor */
  VertexArray(VertexArray const&) = delete;
  VertexArray& operator=(VertexArray const&) = delete;

  /* Initialize vertex/index buffers with data */
  void InitializeBuffers(const VertexBufferLayout& layout, const VertexBufferData& data);

  void Bind() const;

  void Unbind() const;
  
  /* Free resources from GPU */
  void Destroy() const;

  const uint32_t& VertexArrayID() const { return _vao; }
  const uint32_t& VertexBufferID() const { return _vbo; }
  const uint32_t& IndexBufferID() const { return _ebo; }
  const VertexBufferLayout& GetLayout() const { return _layout; }
  
  /* Copy the vertex buffer data into writeBuffer */
  void CopyVertexBufferData(uint32_t writeBuffer);

  /* Copy the index buffer data into writeBuffer */
  void CopyIndexBufferData(uint32_t writeBuffer);

  uint32_t numVertices; /* Used in renderer */
  uint32_t numIndices;  /* Used in renderer */
  uint32_t vboSize;     /* Cache the vertex buffer size (in byte) */
  
private:
  VertexBufferLayout _layout; /* Used in instancing */
  uint32_t _vao;
  uint32_t _vbo;
  uint32_t _ebo;
};

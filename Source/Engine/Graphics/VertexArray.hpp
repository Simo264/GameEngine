#pragma once

#include "Core/Core.hpp"

/* ------------------------------------ 
      Vertex buffer configuration 
  ------------------------------------ */
class VertexBufferConfig
{
public:
  VertexBufferConfig()
  {
    numAttrs = 0;
    layout.fill(0);
  }
  
  void PushAttribute(uint8_t attribute)
  {
    layout[numAttrs++] = attribute;
  }
  
  void PushAttributes(std::initializer_list<uint8_t> values) 
  {
    for (int i = numAttrs; i < values.size(); i++, numAttrs++)
      layout[i] = *(values.begin() + i);
  }

  /* 
    ex: layout = [3, 3, 2]
    layout=0: in vec3 components (x,y,z)
    layout=1: in vec3 components (x,y,z)
    layout=2: in vec2 components (x,y)
  */
  Array<uint8_t, 16> layout;
  uint32_t numAttrs;
};

/* ------------------------------------
      Vertex array data class
  ------------------------------------ */
struct VertexArrayData
{
  uint64_t	vertDataSize; /* size in bytes */
  float*    vertData;
  uint64_t	indDataSize; /* size in bytes */
  uint32_t* indData;

  VertexArrayData() : 
    vertDataSize{ 0 }, 
    vertData{ nullptr }, 
    indDataSize{ 0 }, 
    indData{ nullptr } 
  {}
  VertexArrayData(uint64_t vSize, float* vData, uint64_t iSize, uint32_t* iData) : 
    vertDataSize{ vSize }, 
    vertData{ vData }, 
    indDataSize{ iSize }, 
    indData{ iData } 
  {}
};


/* ------------------------------------
      Vertex array class
  ------------------------------------ */
class VertexArray
{
public:
  VertexArray(const VertexArrayData& data, const VertexBufferConfig& config);

  /* Disable copy constructor */
  VertexArray(VertexArray const&) = delete;
  VertexArray& operator=(VertexArray const&) = delete;

  void Bind() const;
  void Unbind() const;
  void Destroy() const;

  const uint32_t& VertexArrayID() const { return _vao; }
  const uint32_t& VertexBufferID() const { return _vbo; }
  const uint32_t& IndexBufferID() const { return _ebo; }
  const VertexBufferConfig& GetConfig() const { return _config; }
  
  /* Copy the vertex buffer data into writeBuffer */
  void CopyVertexBufferData(uint32_t writeBuffer);

  /* Copy the index buffer data into writeBuffer */
  void CopyIndexBufferData(uint32_t writeBuffer);

  uint32_t numVertices; /* Used in renderer */
  uint32_t numIndices;  /* Used in renderer */
  uint32_t vboSize;     /* Cache the vertex buffer size (in byte) */
  
private:
  VertexBufferConfig _config; /* Used in instancing */
  uint32_t _vao;
  uint32_t _vbo;
  uint32_t _ebo;
  
  void InitVertexArray(const VertexArrayData& data, const VertexBufferConfig& config);
};

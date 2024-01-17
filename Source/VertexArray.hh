#pragma once

#include "Core.hh"
#include "NonCopyable.hh"

// Vertex Array Configuration 
// -------------------------------------------------
class VertexArrayConfig
{
public:
  VertexArrayConfig();
  ~VertexArrayConfig() = default;
  void PushAttribute(uint8_t attribute);
  void PushAttributes(std::initializer_list<uint8_t> values);

  // ex: layout = [4, 3, 2]
  // layout=0: in vec4 components (x,y,z,w)
  // layout=1: in vec3 components (x,y,z)
  // layout=2: in vec2 components (x,y)
  Array<uint8_t, 16> layout;
  uint32_t numAttrs;
};


// Vertex Array Data
// -------------------------------------------------
struct VertexArrayData
{
  uint64_t	vertDataSize;
  float*    vertData;
  uint64_t	indDataSize;
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


// Vertex Array Object
// -------------------------------------------------
class VertexArray : public NonCopyable
{
public:
  VertexArray();

  void InitVertexArray(VertexArrayData& data, VertexArrayConfig& config);
  void DestroyVertexArray();

  void BindVertexArray()   const { glBindVertexArray(_vao); };
  void UnbindVertexArray() const { glBindVertexArray(0); };

  const VertexArrayConfig& GetConfig() const { return _config; }

  uint32_t numVertices;
  uint32_t numIndices;

private:
  VertexArrayConfig _config;
  
  uint32_t _vao; 
  uint32_t _vbo;
  uint32_t _ebo;
};

#pragma once

#include "../Core.hh"
#include "../UncopyableObject.hh"

// Vertex Array Configuration 
// -------------------------------------------------
class VertexArrayConfig
{
public:
  VertexArrayConfig()
  {
    numAttrs = 0;
    layout.fill(0);
  }

  VertexArrayConfig(std::initializer_list<uint8_t> values)
  {
    numAttrs = values.size();
    layout.fill(0);
    std::copy(values.begin(), values.end(), layout.begin());
  }

  void PushAttribute(uint8_t attribute);

  // ex: layout = [3, 3, 2]
  // layout=0: in vec3 components (x,y,z)
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

  VertexArrayData()
    : vertDataSize{ 0 }, indDataSize{ 0 }, vertData{ nullptr }, indData{ nullptr } {}
  VertexArrayData(uint64_t vSize, float* vData, uint64_t iSize, uint32_t* iData)
    : vertDataSize{ vSize }, indDataSize{ iSize }, vertData{ vData }, indData{ iData } {}
};


// Vertex Array Object
// -------------------------------------------------
class VertexArray : public UncopyableObject
{
public:
  VertexArray();

  void InitVertexArray(VertexArrayData& data, VertexArrayConfig& config);
  void DestroyVertexArray();

  void BindVertexArray()   const { glBindVertexArray(_vao); };
  void UnbindVertexArray() const { glBindVertexArray(0); };

  uint32_t numVertices;
  uint32_t numIndices;

private:
  uint32_t _vao; 
  uint32_t _vbo;
  uint32_t _ebo;
};

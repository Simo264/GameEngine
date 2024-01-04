#pragma once

#include "../Core.hh"
#include "../Logger.hh"

#include <stack>

// Vertex Array Configuration 
// --------------------------------------
struct VAConfig
{
  VAConfig()
  {
    numAttrs = 0;
    layout.fill(0);
  }

  void PushAttribute(uint8_t attribute)
  {
    if (numAttrs >= layout.size())
    {
      CONSOLE_ERROR("VAConfiguration::pushAttribute can't push more attributes");
      return;
    }
    layout[numAttrs++] = attribute;
  }

  // ex: layout = [3, 3, 2]
  // layout=0: in vec3 components (x,y,z)
  // layout=1: in vec3 components (x,y,z)
  // layout=2: in vec2 components (x,y)
  Array<uint8_t, 16> layout;
  uint32_t numAttrs;
};


// Vertex Array Data
// ----------------------------------------
struct VAData
{
  uint32_t	vertDataSize;
  float*    vertData;

  uint32_t	indDataSize;
  uint32_t* indData;

  VAData()
    : vertDataSize{ 0 }, indDataSize{ 0 }, vertData{ nullptr }, indData{ nullptr } {}
  VAData(uint32_t vSize, float* vData, uint32_t iSize, uint32_t* iData)
    : vertDataSize{ vSize }, indDataSize{ iSize }, vertData{ vData }, indData{ iData } {}
};


// Vertex Array Object
// -------------------------------------------------
class VertexArray
{
public:
  VertexArray() : _ebo{ 0 }, numVertices{ 0 }, numIndices{ 0 } {}
  ~VertexArray() = default;

  VertexArray(const VertexArray&) = delete;            // delete copy constructor
  VertexArray& operator=(const VertexArray&) = delete; // delete assign op

  void Create(VAData& data, VAConfig& config);
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

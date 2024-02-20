#pragma once

#include "Core.hh"
#include "NonCopyable.hh"

/* ------------------------------------ 
      Vertex array configuration class
  ------------------------------------ */
class VertexArrayConfig
{
public:
  VertexArrayConfig();
  ~VertexArrayConfig() = default;
  void PushAttribute(uint8_t attribute);
  void PushAttributes(std::initializer_list<uint8_t> values);

  /* 
    ex: layout = [4, 3, 2]
    layout=0: in vec4 components (x,y,z,w)
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
class VertexArray : public NonCopyable
{
public:
  VertexArray();

  void InitVertexArray(VertexArrayData& data, VertexArrayConfig& config);
  void DestroyVertexArray() const;

  void BindVertexArray()   const { glBindVertexArray(_vao); };
  void UnbindVertexArray() const { glBindVertexArray(0); };

  const uint32_t& VertexArrayID() const { return _vao; }
  const uint32_t& VertexBufferID() const { return _vbo; }
  const uint32_t& IndexBufferID() const { return _ebo; }
  const VertexArrayConfig& GetConfig() const { return _config; }
  
  /* Copy the vertex buffer data into writeBuffer */
  void CopyVertexBufferData(uint32_t writeBuffer);

  /* Copy the index buffer data into writeBuffer */
  void CopyIndexBufferData(uint32_t writeBuffer);

  uint32_t numVertices; /* Used in renderer */
  uint32_t numIndices;  /* Used in renderer */
  uint32_t vboSize;     /* Cache the vertex buffer size (in byte) */
  
private:
  VertexArrayConfig _config; /* Used in instancing */
  uint32_t _vao;
  uint32_t _vbo;
  uint32_t _ebo;
};

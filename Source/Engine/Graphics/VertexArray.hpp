#pragma once

#include "Core/Core.hpp"
#include "Engine/Graphics/Buffers/VertexBuffer.hpp"
#include "Engine/Graphics/Buffers/ElementBuffer.hpp"

constexpr int MAX_VERTEX_ATTRIBUTES = 128;

/**  
 *  -------------------------- Vertex Specification --------------------------
 *  Vertex Specification is the process of setting up the necessary objects for 
 *  rendering with a particular shader program, as well as the process of using 
 *  those objects to render. 
 *  
 *  Submitting vertex data for rendering requires creating a stream of vertices, 
 *  and then telling OpenGL how to interpret that stream.
 *
 *  -------------------------- Vertex Stream --------------------------
 *  In order to render at all, you must be using a shader program or program pipeline 
 *  which includes a Vertex Shader. 
 *  
 *  The Vertex Shader defines the list of expected Vertex Attributes for that shader, 
 *  where each attribute is mapped to each user-defined input variable. 
 *  This set of attributes defines what values the vertex stream must provide to properly 
 *  render with this shader.
 *
 *  There are two ways of rendering with arrays of vertices: 
 *    1. you can generate a stream in the array's order (without index buffer)
 *    2. or you can use a list of indices to define the order (with index buffer)
 *
 * 
 *  Let's say you have the following array of arrays containing 3d position data belonging to 3 vertices:
 *  { {1, 1, 1}, {0, 0, 0}, {0, 0, 1} }
 * 
 *  A vertex stream can of course have multiple attributes. 
 *  You can take the above position array and augment it with, for example, a texture coordinate array:
 *  { {0, 0}, {0.5, 0}, {0, 1} }
 * 
 *  The vertex stream you get will be as follows:
 *  { 
 *    [{0, 0, 1}, {0.0, 1}], 
 *    [{0, 0, 0}, {0.5, 0}], 
 *    [{1, 1, 1}, {0.0, 0}], 
 *    [{0, 0, 1}, {0.0, 1}], 
 *    [{0, 0, 0}, {0.5, 0}], 
 *    [{0, 0, 1}, {0.0, 1}] 
 *  }
 *  
 *  The above stream is not enough to actually draw anything; you must also tell OpenGL how to interpret this stream.
 */
class VertexSpecification
{
public:

  /**
   *  e.g: from vertex data stream
   *  {   
   *    [0,0,1, 0,0,0, 1,0] -> vec3, vec3, vec2
   *    [0,0,1, 0,0,0, 0,1] -> vec3, vec3, vec2
   *  }
   *
   *  We describe that stream of data using two arrays:
   *    1. attributes array [3, 3, 2] 
   *    2. locations array  [0, 1, 2]
   * 
   *    layout (location=0) in vec3 attribute0 (x,y,z)
   *    layout (location=1) in vec3 attribute1 (x,y,z)
   *    layout (location=2) in vec2 attribute2 (x,y)
   */
  VertexSpecification(int numAttributes) 
  {
    _vertexAttributes = numAttributes;
    
    /* By default attribute array is set to 0 -> [0, 0, 0] */
    _attributes.fill(0);

    /* By default location array is incremental -> [0, 1, 2] */
    for (int i = 0; i < numAttributes; i++)
      _locations[i] = i;
  }

  /**
   * Update the attribute array with the given values
   */
  void UpdateAttributes(std::initializer_list<uint8_t> values)
  {
    for (int i = 0; i < values.size(); i++)
      _attributes[i] = *(values.begin() + i);
  }

  /**
   * Update the location array with the given values
   */
  void UpdateLocations(std::initializer_list<uint8_t> values)
  {
    for (int i = 0; i < values.size(); i++)
      _locations[i] = *(values.begin() + i);
  }
 
  uint32_t GetNumAttributes() const { return _vertexAttributes; }
  const Array<uint8_t, MAX_VERTEX_ATTRIBUTES>& GetAttributes() { return _attributes; }
  const Array<uint8_t, MAX_VERTEX_ATTRIBUTES>& GetLocations() { return _locations; }

private:
  Array<uint8_t, MAX_VERTEX_ATTRIBUTES> _attributes;
  Array<uint8_t, MAX_VERTEX_ATTRIBUTES> _locations;
  uint32_t _vertexAttributes;
};



/**
 *  A Vertex Array Object is an OpenGL Object that stores all of the state needed to supply vertex data.
 *  It stores the format of the vertex data. 
 *
 *  As OpenGL Objects, VAOs have the usual creation, destruction, and binding functions: 
 *    - glGenVertexArrays
 *    - glDeleteVertexArrays
 *    - glBindVertexArray
 *
 *  Vertex attributes are numbered from 0 to GL_MAX_VERTEX_ATTRIBS - 1.
 *  Each attribute can be enabled or disabled for array access:
 *  when an attribute's array access is disabled, any reads of that attribute by the vertex shader will produce 
 *  a constant value instead of a value pulled from an array.
 *  
 *  A newly-created VAO has array access disabled for all attributes. 
 *  Array access is enabled by binding the VAO in question and calling glEnableVertexAttribArray(index​);
 */
class VertexArray
{
public:
  VertexArray();
  
  ~VertexArray() = default;

  /**
   * Generate vertex array object names
   */
  void Generate();

  /**
   * Only delete the vertex array object
   */
  void Delete() const;

  /**
   * Delete either the vertex array object and all buffer objects bounded with it
   */
  void Destroy() const;

  /**
   * Bind the vertex array object
   */
  void Bind() const;

  /**
   * Unbind the vertex array object
   */
  void Unbind() const;

  /**
   * Bind a buffer to a vertex buffer bind point
   * 
   * @param bindingindex: the index of the vertex buffer binding point to which to bind the buffer
   * @param offset:       the offset of the first element of the buffer
   * @param stride:       the distance between elements within the buffer
   */
  void BindVertexBuffer(const VertexBuffer& vertexBuffer, int bindingindex, int offset, int stride) const;

  /**
   * Enable a generic vertex attribute array
   * 
   * @param index: specifies the index of the generic vertex attribute to be enabled or disabled.
   */
  void EnableAttribute(int index) const;

  /**
   * Disable a generic vertex attribute array
   *
   * @param index: specifies the index of the generic vertex attribute to be enabled or disabled.
   */
  void DisableAttribute(int index) const;

  /**
   * Specify the organization of vertex arrays
   *
   * @param attribindex:    the generic vertex attribute array being described
   * @param size:           the number of values per vertex that are stored in the array
   * @param relativeoffset: the distance between elements within the buffer
   */
  void SetAttribSpecification(int attribindex, int size, int type, bool normalize, int relativeoffset) const;

  /**
   * associate a vertex attribute and a vertex buffer binding for a vertex array object
   */
  void SetAttribBinding(int attribindex, int bindingindex) const;

private:
  uint32_t _vao;

  VertexBuffer _vbo;
  ElementBuffer _ebo;
};




#if 0

/* ------------------------------------
      Vertex array class
  ------------------------------------ */
class VertexArray
{
public:
  /* Create buffer objects.
    
    usage: specifies the expected usage pattern of the data store. 
      The symbolic constant must be one of these:
      GL_STREAM_DRAW, GL_STREAM_READ, GL_STREAM_COPY,   GL_STATIC_DRAW, 
      GL_STATIC_READ, GL_STATIC_COPY, GL_DYNAMIC_DRAW,  GL_DYNAMIC_READ, GL_DYNAMIC_COPY.
  */
  VertexArray();

  /*
  Create and initialize buffer objects.

  usage: specifies the expected usage pattern of the data store. 
      The symbolic constant must be one of these:
      GL_STREAM_DRAW, GL_STREAM_READ, GL_STREAM_COPY,   GL_STATIC_DRAW, 
      GL_STATIC_READ, GL_STATIC_COPY, GL_DYNAMIC_DRAW,  GL_DYNAMIC_READ, GL_DYNAMIC_COPY.
  */
  VertexArray(const VertexBufferLayout& layout, const VertexBufferData& data, uint32_t usage);

  /* The destructor do nothing. You must call Destroy to free memory */
  ~VertexArray() = default;

  /* Disable copy constructor */
  VertexArray(VertexArray const&) = delete;
  VertexArray& operator=(VertexArray const&) = delete;

  /* Initialize vertex/index buffers with data */
  void InitializeBuffers(const VertexBufferLayout& layout, const VertexBufferData& data, uint32_t usage);

  void Bind() const;

  void Unbind() const;
  
  /* Free resources from GPU */
  void Destroy() const;

  uint32_t VertexArrayID()  const { return _vao; }
  uint32_t VertexBufferID() const { return _vbo; }
  uint32_t IndexBufferID()  const { return _ebo; }
  VertexBufferLayout GetLayout() const { return _layout; }
  

  uint32_t numVertices; /* Used in renderer */
  uint32_t numIndices;  /* Used in renderer */
  uint32_t vboSize;     /* Cache the vertex buffer size (in byte) */
  
private:
  VertexBufferLayout _layout; /* Used in instancing */
  uint32_t _vao;              /* Vertex array object */
  uint32_t _vbo;              /* Vertex buffer object */
  uint32_t _ebo;              /* Element (index) buffer object */
};
#endif
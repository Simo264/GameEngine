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
struct VertexSpecification
{
  
  
};



/**
 *  A Vertex Array Object is an OpenGL Object that stores all of the state needed to supply vertex data.
 *  It stores the format of the vertex data. 
 *
 *  Vertex attributes are numbered from 0 to GL_MAX_VERTEX_ATTRIBS - 1.
 * 
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
  VertexArray() : vao{ static_cast<uint32_t>(-1) } {}
  
  /**
   * Generate vertex array object
   */
  void Create();

  /**
   * Delete vertex array object
   */
  void Delete() const;

  /**
   * Bind vertex array object
   */
  void Bind() const;

  /**
   * Unbind vertex array object
   */
  void Unbind() const;

  /**
   * Bind the vertex buffer <vertexBuffer> to the binding point whose index is given by <bindingindex>
   * 
   * @param bindingindex: the index of the vertex buffer binding point to which to bind the buffer
   * 
   * @param offset:       the offset of the first element of the buffer
   * 
   * @param stride:       the distance between elements within the buffer
   */
  void BindVertexBuffer(int bindingindex, const VertexBuffer& vertexBuffer,  int offset, int stride) const;

  /**
   * Enable a generic vertex attribute array
   * 
   * @param attribindex: specifies the index of the generic vertex attribute to be enabled or disabled.
   */
  void EnableAttribute(int attribindex) const;

  /**
   * Disable a generic vertex attribute array
   *
   * @param attribindex: specifies the index of the generic vertex attribute to be enabled or disabled.
   */
  void DisableAttribute(int attribindex) const;

  /**
   * Specify the organization of vertex arrays
   *
   * @param attribindex:    specifies the index of the generic vertex attribute array whose data layout is being described
   * 
   * @param size:           determines the number of components per vertex are allocated to the specified attribute 
   *                        and must be 1, 2, 3, 4, or GL_BGRA
   * 
   * @param type:           indicates the type of the data. If type is one of
   *                        GL_BYTE, GL_SHORT, GL_INT, GL_FIXED, GL_FLOAT, GL_HALF_FLOAT, and GL_DOUBLE 
   *                        indicate types GLbyte, GLshort, GLint, GLfixed, GLfloat, GLhalf, and GLdouble, 
   *                        respectively; the values GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT, and GL_UNSIGNED_INT 
   *                        indicate types GLubyte, GLushort, and GLuint, respectively; 
   *                        the values GL_INT_2_10_10_10_REV and GL_UNSIGNED_INT_2_10_10_10_REV indicating respectively 
   *                        four signed or unsigned elements packed into a single GLuint; 
   *                        and the value GL_UNSIGNED_INT_10F_11F_11F_REV indicating three floating point values packed 
   *                        into a single GLuint.
   * 
   * @param normalized:     if normalized is GL_TRUE, then integer data is normalized to the range [-1, 1] or [0, 1] 
   *                        if it is signed or unsigned, respectively. 
   *                        If normalized is GL_FALSE then integer data is directly converted to floating point. 
   * 
   * @param relativeoffset: is the offset of the first element relative to the start of the vertex buffer binding 
   *                        this attribute fetches from.
   */
  void SetAttribFormat(int attribindex, int size, int type, bool normalize, int relativeoffset) const;


  /**
   * Establishes an association between the generic vertex attribute of a vertex array object 
   * whose index is given by attribindex, and a vertex buffer binding whose index is given by bindingindex
   */
  void SetAttribBinding(int attribindex, int bindingindex) const;

  VertexBuffer  vertexBuffer;
  ElementBuffer elementBuffer;

  uint32_t vao;
};


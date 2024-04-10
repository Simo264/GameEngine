#pragma once

#include "Core/Core.hpp"
#include "Engine/Graphics/VertexBuffer.hpp"
#include "Engine/Graphics/ElementBuffer.hpp"

constexpr int MAX_VERTEX_ATTRIBUTES = 128;

/**  
 *  https://www.khronos.org/opengl/wiki/Vertex_Specification 
 * 
 *  Vertex Specification is the process of setting up the necessary objects for 
 *  rendering with a particular shader program, as well as the process of using 
 *  those objects to render. 
 *  
 *  Submitting vertex data for rendering requires creating a stream of vertices, 
 *  and then telling OpenGL how to interpret that stream
 */
struct VertexSpecifications
{
  /**
   * The index of the vertex buffer binding point to which to bind the buffer
   */
  int bindingindex;
  
  /**
   * The index of the generic vertex attribute to be enabled or disabled.
   */
  int attrindex;

  /**
   * The number of components per vertex are allocated to the specified attribute and must be 1, 2, 3, 4, or GL_BGRA
   */
  int components;

  /**
   * Indicates the type of the data.
   */
  int type;

  /**
   * If true then integer data is normalized to the range [-1, 1] or [0, 1] if it is signed or unsigned, respectively.
   * Otherwise if normalized is false then integer data is directly converted to floating point
   */
  bool normalized;

  /**
   * Is the offset of the first element relative to the start of the vertex buffer binding this attribute fetches from
   */
  int relativeoffset;
};


/**
 *  https://www.khronos.org/opengl/wiki/Vertex_Specification#Vertex_Array_Object
 * 
 *  A Vertex Array Object is an OpenGL Object that stores all of the state needed to supply vertex data.
 *  It stores the format of the vertex data. 
 *
 *  Vertex attributes are numbered from 0 to GL_MAX_VERTEX_ATTRIBS - 1.
 * 
 *  Each attribute can be enabled or disabled for array access:
 *  when an attribute's array access is disabled, any reads of that attribute by the vertex shader will produce 
 *  a constant value instead of a value pulled from an array.
 */
class VertexArray
{
public:
  VertexArray();
  ~VertexArray() = default;

  /**
   * Generate vertex array object
   */
  void Create();

  /**
   * Delete vertex array object and all attachments
   */
  void Delete();

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
   * @param offset:       the offset of the first element of the buffer
   * @param stride:       the distance between elements within the buffer
   */
  void AttachVertexBuffer(int bindingindex, VertexBuffer& buffer, int offset, int stride);

  /**
   * Configures element array buffer binding of a vertex array object
   */
  void AttachElementBuffer(ElementBuffer& buffer);

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

  void SetVertexSpecifications(const VertexSpecifications& specs) const;

  uint32_t id;

  int numVertices;
  int numIndices;

  ElementBuffer         eboAttachment;
  Vector<VertexBuffer>  vboAttachments;

};


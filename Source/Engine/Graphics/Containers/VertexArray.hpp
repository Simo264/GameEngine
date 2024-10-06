#pragma once

#include "Core/Core.hpp"
#include "Engine/Graphics/Objects/Buffer.hpp"

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
   * The index of the vertex buffer binding poi32 to which to bind the buffer
   */
  i32 bindingindex;
  
  /**
   * The index of the generic vertex attribute to be enabled or disabled.
   */
  i32 attrindex;

  /**
   * The number of components per vertex are allocated to the specified attribute and must be 1, 2, 3, 4, or GL_BGRA
   */
  i32 components;

  /**
   * Indicates the type of the data.
   */
  i32 type;

  /**
   * If true then integer data is normalized to the range [-1, 1] or [0, 1] if it is signed or unsigned, respectively.
   * Otherwise if normalized is false then integer data is directly converted to floating point
   */
  bool normalized;

  /**
   * Is the offset of the first element relative to the start of the vertex buffer binding this attribute fetches from
   */
  i32 relativeoffset;
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
  VertexArray() : 
    id{ 0 },
    numIndices{ 0 },
    numVertices{ 0 },
    eboAttachmentID{ 0 },
    vboAttachmentIDs{}
  {}
  ~VertexArray() = default;

  /**
   * Generate vertex array object
   */
  void Create();

  /**
   * Delete vertex array object and all attachments and invalidates the name associated with the vertex array object 
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
   * Bind the vertex buffer to the binding poi32 whose index is given by bindingindex
   * 
   * @param bindingindex: the index of the vertex buffer binding poi32 to which to bind the buffer
   * @param offset:       the offset of the first element of the buffer
   * @param stride:       the distance between elements within the buffer
   */
  void AttachVertexBuffer(i32 bindingindex, u32 bufferID, i32 offset, i32 stride);

  /**
   * Configures element array buffer binding of a vertex array object
   */
  void AttachElementBuffer(u32 bufferID);

  /**
   * Enable a generic vertex attribute array
   * 
   * @param attribindex: specifies the index of the generic vertex attribute to be enabled or disabled.
   */
  void EnableAttribute(i32 attribindex) const;

  /**
   * Disable a generic vertex attribute array
   *
   * @param attribindex: specifies the index of the generic vertex attribute to be enabled or disabled.
   */
  void DisableAttribute(i32 attribindex) const;

  /**
   * Specify the organization of vertex arrays
   *
   * @param attribindex:    specifies the index of the generic vertex attribute array whose data layout is being described
   * 
   * @param size:           determines the number of components per vertex are allocated to the specified attribute 
   *                        and must be 1, 2, 3, 4, or GL_BGRA
   * @param type:           indicates the type of the data. If type is one of
   *                        GL_BYTE, GL_SHORT, GL_INT, GL_FIXED, GL_FLOAT, GL_HALF_FLOAT, and GL_DOUBLE 
   *                        indicate types GLbyte, GLshort, GLint, GLfixed, GLfloat, GLhalf, and GLdouble, 
   *                        respectively; the values GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT, and GL_UNSIGNED_INT 
   *                        indicate types GLubyte, GLushort, and GLuint, respectively; 
   *                        the values GL_INT_2_10_10_10_REV and GL_UNSIGNED_INT_2_10_10_10_REV indicating respectively 
   *                        four signed or unsigned elements packed into a single GLuint; 
   *                        and the value GL_UNSIGNED_INT_10F_11F_11F_REV indicating three floating poi32 values packed 
   *                        into a single GLuint.
   * @param normalized:     if normalized is GL_TRUE, then integer data is normalized to the range [-1, 1] or [0, 1] 
   *                        if it is signed or unsigned, respectively. 
   *                        If normalized is GL_FALSE then integer data is directly converted to floating point. 
   * @param relativeoffset: is the offset of the first element relative to the start of the vertex buffer binding 
   *                        this attribute fetches from.
   */
  void SetAttribFormat(i32 attribindex, i32 size, i32 type, bool normalize, i32 relativeoffset) const;

  /**
   * Establishes an association between the generic vertex attribute of a vertex array object 
   * whose index is given by attribindex, and a vertex buffer binding whose index is given by bindingindex
   */
  void SetAttribBinding(i32 attribindex, i32 bindingindex) const;

  /**
   * Modify the rate at which generic vertex attributes advance
   * 
   * @param bindingindex: the index of the binding whose divisor to modify
   * @param divisor: the new value for the instance step rate to apply
   */
  void SetBindingDivisor(i32 bindingindex, i32 divisor) const;

  void SetVertexSpecifications(const VertexSpecifications& specs) const;

  u32 id;

  u32 numVertices;
  u32 numIndices;

  u32 eboAttachmentID;
  Vector<u32> vboAttachmentIDs;
};


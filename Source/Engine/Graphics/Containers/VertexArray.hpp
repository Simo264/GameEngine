#pragma once

#include "Core/Core.hpp"
#include "Engine/Graphics/Objects/Buffer.hpp"

/**
 * @brief
 * https://www.khronos.org/opengl/wiki/Vertex_Specification#Vertex_Array_Object
 * 
 * A Vertex Array Object is an OpenGL Object that stores all of the state needed to supply vertex data.
 * It stores the format of the vertex data. 
 *
 * Vertex attributes are numbered from 0 to GL_MAX_VERTEX_ATTRIBS - 1.
 * 
 * Each attribute can be enabled or disabled for array access:
 * when an attribute's array access is disabled, any reads of that attribute by the vertex shader will produce 
 * a constant value instead of a value pulled from an array.
 */
class VertexArray
{
public:
  VertexArray() : 
    id{ 0 },
    numIndices{ 0 },
    numVertices{ 0 },
    eboAttachment{},
    vboAttachments{}
  {}
  ~VertexArray() = default;

  /** @brief Generate vertex array object. */
  void Create();

  /**
   * @brief
   * Delete vertex array object and all attachments and invalidates the name associated with the vertex array object 
   */
  void Delete();

  /** @brief Bind vertex array object. */
  void Bind() const;

  /** @brief Unbind vertex array object. */
  void Unbind() const;

  /**
   * @brief
   * Bind the vertex buffer to the binding poi32 whose index is given by bindingindex
   * 
   * @param bindingindex: the index of the vertex buffer binding poi32 to which to bind the buffer
   * @param offset:       the offset of the first element of the buffer
   * @param stride:       the distance between elements within the buffer
   */
  void AttachVertexBuffer(i32 bindingindex, Buffer buffer, i32 offset, i32 stride);

  /** @brief Configures element array buffer binding of a vertex array object */
  void AttachElementBuffer(Buffer buffer);

  /**
   * @brief
   * Enable a generic vertex attribute array
   * 
   * @param attribindex: specifies the index of the generic vertex attribute to be enabled or disabled.
   */
  void EnableAttribute(i32 attribindex) const;

  /**
   * @brief
   * Disable a generic vertex attribute array
   *
   * @param attribindex: specifies the index of the generic vertex attribute to be enabled or disabled.
   */
  void DisableAttribute(i32 attribindex) const;

  /**
   * @brief
   * Specify the organization of vertex arrays. 
   * glVertexArrayAttribFormat should be used to describe vertex attribute layout for floating-point vertex attributes.
   * See https://registry.khronos.org/OpenGL-Refpages/gl4/html/glVertexAttribFormat.xhtml
   */
  void SetAttribFormatFLoat(i32 attribindex, i32 size, i32 type, bool normalize, i32 relativeoffset) const;
  
  /**
   * @brief
   * Specify the organization of vertex arrays. 
   * glVertexArrayAttribIFormat should be used to describe vertex attribute layout for integer vertex attribute.
   * 
   * Data for an array specified by glVertexAttribIFormat and glVertexArrayAttribIFormat will always be left as integer values; 
   * such data are referred to as pure integers.
   * 
   * You cannot use floating-point types such as GL_FLOAT or GL_DOUBLE.
   * 
   * See https://registry.khronos.org/OpenGL-Refpages/gl4/html/glVertexAttribFormat.xhtml
   */
  void SetAttribFormatInteger(i32 attribindex, i32 size, i32 type, i32 relativeoffset) const;

  /**
   * @brief
   * Specify the organization of vertex arrays. 
   * glVertexArrayAttribLFormat is used to specify layout for data associated with a generic 
   * attribute variable declared as 64-bit double precision components.
   * 
   * For glVertexArrayAttribLFormat, type must be GL_DOUBLE.
   * 
   * See https://registry.khronos.org/OpenGL-Refpages/gl4/html/glVertexAttribFormat.xhtml
   */
  void SetAttribFormatLong(i32 attribindex, i32 size, i32 relativeoffset) const;

  /**
   * @brief
   * Establishes an association between the generic vertex attribute of a vertex array object 
   * whose index is given by attribindex, and a vertex buffer binding whose index is given by bindingindex
   */
  void SetAttribBinding(i32 attribindex, i32 bindingindex) const;

  /**
   * @brief
   * Modify the rate at which generic vertex attributes advance
   * 
   * @param bindingindex: the index of the binding whose divisor to modify
   * @param divisor: the new value for the instance step rate to apply
   */
  void SetBindingDivisor(i32 bindingindex, i32 divisor) const;

  bool IsValid() const;

  u32 id;
  u32 numVertices;
  u32 numIndices;

  /* A VAO can only have one index buffer associated with it at any given time */
  Buffer eboAttachment;

  /**
   * A VAO can have several vertex buffers associated with it at the same time (MAX 16).
   * This is handled via the mechanism of attribute binding points.
   */
  inline static constexpr i32 MAX_NUM_VBO_ATTACHMENTS = 16;
  Vector<Buffer> vboAttachments;
};
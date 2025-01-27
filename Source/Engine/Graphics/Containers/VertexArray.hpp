#pragma once

#include "Core/Core.hpp"
#include "Engine/Graphics/Objects/Buffer.hpp"

enum class VertexAttribType : u32
{
  Byte = 0x1400, // GL_BYTE
  UnsignedByte = 0x1401, // GL_UNSIGNED_BYTE
  Short = 0x1402, // GL_SHORT
  UnsignedShort = 0x1403, // GL_UNSIGNED_SHORT
  Int = 0x1404, // GL_INT
  UnsignedInt = 0x1405, // GL_UNSIGNED_INT
  Float = 0x1406, // GL_FLOAT
  Double = 0x140A, // GL_DOUBLE
  HalfFloat = 0x140B, // GL_HALF_FLOAT
  UnsignedByte_4_4_4_4 = 0x8033, // GL_UNSIGNED_BYTE_4_4_4_4
  UnsignedByte_5_5_5_1 = 0x8034, // GL_UNSIGNED_BYTE_5_5_5_1
  UnsignedByte_5_6_5 = 0x8D61  // GL_UNSIGNED_BYTE_5_6_5
};


/**
 * @struct VertexFormat
 * @brief Defines the structure of a vertex attribute in a vertex buffer.
 */
struct VertexFormat
{
  /** @brief Number of components per vertex attribute. Must be 1, 2, 3, 4, or GL_BGRA.*/
  i32 size{ 0 };

  /** @brief Specifies the data type of the attribute. */
  VertexAttribType type{};

  /**
   * @brief Specifies if integer data should be normalized to [0, 1] or [-1, 1].
   * If true, signed/unsigned integers are normalized. If false, data is converted directly to float.
   */
  bool normalized{ false };

  /** @brief Offset of the attribute relative to the start of the vertex buffer. */
  i32 relativeoffset{ 0 };
};

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
   * @brief Disable a generic vertex attribute array
   *
   * @param attribindex: specifies the index of the generic vertex attribute to be enabled or disabled.
   */
  void DisableAttribute(i32 attribindex) const;

  /**
   * @brief Specify the organization of vertex arrays. 
   * glVertexArrayAttribFormat should be used to describe vertex attribute layout for floating-point vertex attributes.
   */
  void SetAttribFormatFLoat(i32 attribindex, i32 size, VertexAttribType type, bool normalize, i32 relativeoffset) const;
  
  /**
   * @brief Specify the organization of vertex arrays. 
   * glVertexArrayAttribIFormat should be used to describe vertex attribute layout for integer vertex attribute.
   * 
   * Data for an array specified by glVertexAttribIFormat and glVertexArrayAttribIFormat will always be left as integer values; 
   * such data are referred to as pure integers.
   * 
   * You cannot use floating-point types such as GL_FLOAT or GL_DOUBLE.
   */
  void SetAttribFormatInteger(i32 attribindex, i32 size, VertexAttribType type, i32 relativeoffset) const;

  /**
   * @brief Specify the organization of vertex arrays. 
   * glVertexArrayAttribLFormat is used to specify layout for data associated with a generic 
   * attribute variable declared as 64-bit double precision components.
   * 
   * For glVertexArrayAttribLFormat, type must be GL_DOUBLE.
   */
  void SetAttribFormatLong(i32 attribindex, i32 size, i32 relativeoffset) const;

  /**
   * @brief Establishes an association between the generic vertex attribute of a vertex array object 
   * whose index is given by attribindex, and a vertex buffer binding whose index is given by bindingindex
   */
  void SetAttribBinding(i32 attribindex, i32 bindingindex) const;

  /**
   * @brief Modify the rate at which generic vertex attributes advance
   * 
   * @param bindingindex: the index of the binding whose divisor to modify
   * @param divisor: the new value for the instance step rate to apply
   */
  void SetBindingDivisor(i32 bindingindex, i32 divisor) const;

  bool IsValid() const;

  u32 id;
  u32 numVertices;
  u32 numIndices;

  /** @brief A VAO can only have one index buffer associated with it at any given time */
  Buffer eboAttachment;

  /**
   * brief A VAO can have several vertex buffers associated with it at the same time (MAX 16).
   * This is handled via the mechanism of attribute binding points.
   */
  inline static constexpr i32 MAX_NUM_VBO_ATTACHMENTS = 16;
  Vector<Buffer> vboAttachments;
};
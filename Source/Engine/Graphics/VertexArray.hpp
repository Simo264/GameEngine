#pragma once

#include "Core/Core.hpp"

enum class VertexAttribType : u32
{
  // Signed integers
  BYTE = 0x1400,           // 8-bit
  SHORT = 0x1402,          // 16-bit
  INT = 0x1404,            // 32-bit 

  // Unsigned integers
  UNSIGNED_BYTE = 0x1401,  // 8-bit
  UNSIGNED_SHORT = 0x1403, // 16-bit
  UNSIGNED_INT = 0x1405,   // 32-bit

  // Floating point
  HALF_FLOAT = 0x140B,     // 16-bit
  FLOAT = 0x1406,          // 32-bit
  DOUBLE = 0x140A,         // 64-bit
};
struct VertexFormat
{
  u32 size{ 0u };
  VertexAttribType type{};
  bool normalized{ false };
  i32 relativeoffset{ 0 };
};

constexpr auto INVALID_VERTEXARRAY_ID = static_cast<u32>(-1);

/**
 * @brief A Vertex Array Object is an OpenGL Object that stores all of the state needed to supply vertex data.
 */
struct VertexArray
{
  VertexArray() : id{ INVALID_VERTEXARRAY_ID } {}
  ~VertexArray();
  // Move constructor
  VertexArray(VertexArray&& other) noexcept
  {
    id = other.id;
    other.id = INVALID_VERTEXARRAY_ID;
  }
  // Move assignment
  VertexArray& operator=(VertexArray&& other) noexcept;
  // Disable copy
  VertexArray(const VertexArray&) = delete;
  VertexArray& operator=(const VertexArray&) = delete;

  static void DeleteArrays(u32 count, const u32* ids);

  /** @brief Creates a new Vertex Array Object (VAO). */
  void Create();
  /** @brief Release GPU memory. */
  void Release();
  /** @brief Deletes the Vertex Array Object (VAO) and its associated buffers. */
  void Delete();
  /** @brief Bind vertex array object. */
  void Bind() const;
  /** @brief Bind the vertex buffer to the binding poiint whose index is given by bindingindex */
  void AttachVertexBuffer(i32 bindingindex, u32 bufferId, i32 offset, i32 stride) const;
  /** @brief Configures element array buffer binding of a vertex array object */
  void AttachIndexBuffer(u32 bufferId) const;
  /** @brief Enable a generic vertex attribute array */
  void EnableVertexAttribute(i32 attribindex) const;
  /** @brief Disable a generic vertex attribute array */
  void DisableVertexAttribute(i32 attribindex) const;
  /** @brief Describe vertex attribute layout for float-32 vertex attributes. */
  void SetAttribFormatFloat32(i32 attribindex, i32 size, VertexAttribType type, bool normalize, i32 relativeoffset) const;
  /** @brief Describe vertex attribute layout for float-64 vertex attribute. */
  void SetAttribFormatFloat64(i32 attribindex, i32 size, i32 relativeoffset) const;
  /** @brief Describe vertex attribute layout for integer32 vertex attribute. */
  void SetAttribFormatInteger32(i32 attribindex, i32 size, VertexAttribType type, i32 relativeoffset) const;

  /** @brief Configures vertex attribute format, binding, and enables it in one call. */
  void SetupVertexAttribute(i32 attribindex, i32 bindingindex, VertexFormat format) const;
  
  /** @brief Establishes an association between the vertex attribute and a vertex buffer binding */
  void SetAttribBinding(i32 attribindex, i32 bindingindex) const;
  /** @brief Modify the rate at which generic vertex attributes advance */
  void SetBindingDivisor(i32 bindingindex, i32 divisor) const;
  
  auto IsValid() const { return id != INVALID_VERTEXARRAY_ID; }

	u32 id;
};
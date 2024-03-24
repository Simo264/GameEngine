#pragma once

#include "Core/Core.hpp"

/**
 *	Indexed rendering, requires an array of indices; all vertex attributes will use the same index from this index array.
 *	The index array is provided by a Buffer Object bound to the GL_ELEMENT_ARRAY_BUFFER binding point. 
 * 
 *	When a buffer is bound to GL_ELEMENT_ARRAY_BUFFER, all drawing commands of the form gl*Draw*Elements* 
 *	will use indexes from that buffer. 
 * 
 *	Indices can be unsigned bytes, unsigned shorts, or unsigned ints.
 *	The index buffer binding is stored within the VAO. 
 *	If no VAO is bound, then you cannot bind a buffer object to GL_ELEMENT_ARRAY_BUFFER.
 */
class ElementBuffer
{
public:
  using ElementBufferDataType = uint32_t;

	ElementBuffer() = default;

  /**
   * Create a new data store for a buffer object
   *
   * @param usage:  the symbolic constant must be GL_STREAM_DRAW, GL_STREAM_READ, GL_STREAM_COPY, GL_STATIC_DRAW,
   *                GL_STATIC_READ, GL_STATIC_COPY, GL_DYNAMIC_DRAW, GL_DYNAMIC_READ, or GL_DYNAMIC_COPY.
   */
  ElementBuffer(const void* data, uint64_t size, int usage);

  /**
   * Copies only the buffer id
   */
  ElementBuffer(const ElementBuffer& other) { this->_buffer = other.GetBufferID(); }

	~ElementBuffer() = default;

  /**
   * Generate buffer object
   */
  void Generate();

  /**
   * Create a new data store for a buffer object.
   * While creating the new storage, any pre-existing data store is deleted.
   * The new data store is created with the specified size in bytes and usage
   *
   * @param usage:  the symbolic constant must be GL_STREAM_DRAW, GL_STREAM_READ, GL_STREAM_COPY, GL_STATIC_DRAW,
   *                GL_STATIC_READ, GL_STATIC_COPY, GL_DYNAMIC_DRAW, GL_DYNAMIC_READ, or GL_DYNAMIC_COPY
   */
  void BufferData(const void* data, uint64_t size, int usage);

  /**
   * Updates a subset of a buffer object's data store
   *
   * @param offset: specifies the offset (in bytes) into the buffer object's data store where data replacement will begin
   * @param size:   specifies the size in bytes of the data store region being replaced
   */
  void BufferSubData(int offset, uint32_t size, const void* data);

  /**
   * Bind the buffer object
   */
  void Bind() const;

  /**
   * Unbind the buffer object
   */
  void Unbind() const;

  /**
   * Delete buffer objects
   */
  void Delete() const;

  uint32_t GetBufferID() const { return _buffer; }

private:
  uint32_t _buffer;
};
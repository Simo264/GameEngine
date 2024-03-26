#pragma once

#include "Core/Core.hpp"
#include "Buffer.hpp"

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
class ElementBuffer : public Buffer
{
public:
  using ElementBufferDataType = uint32_t;

  /**
   * Create a new data store for a buffer object
   *
   * @param usage:  the symbolic constant must be GL_STREAM_DRAW, GL_STREAM_READ, GL_STREAM_COPY, GL_STATIC_DRAW,
   *                GL_STATIC_READ, GL_STATIC_COPY, GL_DYNAMIC_DRAW, GL_DYNAMIC_READ, or GL_DYNAMIC_COPY.
   */
  ElementBuffer(const void* data, uint64_t size, int usage);

  void BufferData(const void* data, uint64_t size, int usage) override;

  void BufferSubData(int offset, uint32_t size, const void* data) override;

  void Bind() const override;

  void Unbind() const override;
};
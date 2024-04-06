#pragma once

#include "Core/Core.hpp"
#include "Buffer.hpp"

/**
 *	https://www.khronos.org/opengl/wiki/Vertex_Specification#Index_Buffers
 * 
 *  Indexed rendering, requires an array of indices; all vertex attributes will use the same index from this index array.
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
  ElementBuffer() 
    : Buffer()
  {}

  ElementBuffer(uint64_t size, const uint32_t* data, int usage);

  ElementBuffer(const ElementBuffer& other)
  {
    id    = other.id;
    size  = other.size;
  }

  void Bind() const override;

  void Unbind() const override;
};
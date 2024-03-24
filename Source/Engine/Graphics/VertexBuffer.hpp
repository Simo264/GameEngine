#pragma once

#include "Core/Core.hpp"

/** 
 *  A Vertex Buffer Object it is used as a source for vertex array data.
 *  There are two ways to use buffer objects as the source for vertex data:
 *    1. Vertex format
 *    2. Separate attribute format
 *  The two are functionally equivalent.
 *
 *  Note: A call to glBindBuffer to set the GL_ARRAY_BUFFER binding is NOT modifying the current VAO's state!
 *
 *  The format and source buffer for an attribute array can be set by doing the following:
 *  first, the buffer that the attribute data comes from must be bound to GL_ARRAY_BUFFER;
 *  once the buffer is bound, call one of these functions:
 *    1. glVertexAttribPointer
 *    2. glVertexAttribIPointer
 *    3. glVertexAttribLPointer
 *  The glVertexAttribPointer functions state where an attribute index gets its array data from.
 *  But it also defines how OpenGL should interpret that data.
 *
 *  Thus, these functions conceptually do two things:
 *    1. set the buffer object information on where the data comes from
 *    2. and define the format of that data
 *
 *  Each attribute index represents a vector of some type, from 1 to 4 components in length.
 *  The size​ parameter of the glVertexAttribPointer functions defines the number of components
 *  in the vector provided by the attribute array. It can be any number 1-4.
 *
 *  Note: if the vertex shader has fewer components than the attribute provides, then the extras are ignored.
 *        If the vertex shader has more components than the array provides, the extras are given values from
 *        the vector (0, 0, 0, 1) for the missing XYZW components.
 *
 *  The vertex format information above tells OpenGL how to interpret the data.
 *  The format says how big each vertex is in bytes and how to convert it into the values that the attribute
 *  in the vertex shader receives.
 *
 *  But OpenGL needs two more pieces of information before it can find the data:
 *    1. it needs a byte offset from the start of the buffer object to the first element in the array
 *    2. it also needs a stride, which represents how many bytes it is from the start of one element to the start of another.
 */
class VertexBuffer
{
public:
  using VertexBufferDataType = float;

  VertexBuffer() = default;

  /**
   * Create a new data store for a buffer object
   * 
   * @param usage:  the symbolic constant must be GL_STREAM_DRAW, GL_STREAM_READ, GL_STREAM_COPY, GL_STATIC_DRAW, 
   *                GL_STATIC_READ, GL_STATIC_COPY, GL_DYNAMIC_DRAW, GL_DYNAMIC_READ, or GL_DYNAMIC_COPY.
   */
  VertexBuffer(const void* data, uint64_t size, int usage);

  /**
   * Copies only the buffer id
   */
  VertexBuffer(const VertexBuffer& other) { this->_buffer = other.GetBufferID(); }

  /**
   * Do not delete buffer object on destructor. Instead call DeleteBuffer()
   */
  ~VertexBuffer() = default;

  /**
   * Generate buffer object 
   */
  void Generate();

  /**
   * Bind the buffer object
   */
  void Bind() const;

  /**
   * Unbind the buffer object
   */
  void Unbind() const;
  
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
   * Delete buffer objects
   */
  void Delete() const;

  uint32_t GetBufferID() const { return _buffer; }

  /**
   * @return the buffer size in bytes
   */
  uint32_t GetBufferSize() const { return _size; }

private:
  uint32_t _buffer;
  uint32_t _size; 
};
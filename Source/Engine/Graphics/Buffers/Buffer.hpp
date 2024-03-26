#pragma once

#include "Core/Core.hpp"

class Buffer
{
public:
	Buffer()	= default;
	~Buffer() = default;
	

	/**
	 * Generate buffer object
	 */
	void Generate();

	/**
	 * Delete buffer objects
	 */
	void Delete() const;

	/**
	 * Copy all or part of the data store of a buffer object to the data store of another buffer object
	 *
	 * @param size:       specifies the size of the data to be copied from the source buffer object
	 *                    to the destination buffer object.
	 * @param readOffset: specifies the offset within the data store of the source buffer at which
	 *                    data will be read
	 * @para writeOffset: specifies the offset within the data store of the destination buffer
	 *                     at which data will be written.
	 *
	 */
	void Copy(const Buffer& other, uint64_t size, int readOffset = 0, int writeOffset = 0);

	/**
	 * Used to calculate the number of vertices
	 *
	 * @return the buffer size in bytes
	 */
	uint64_t GetBufferSize() const { return _size; }

	uint32_t GetBufferID() const { return _buffer; }

	/**
	 * Bind the buffer object
	 */
	virtual void Bind() const = 0;

	/**
	 * Unbind the buffer object
	 */
	virtual void Unbind() const = 0;


	/**
	 * Updates a subset of a buffer object's data store
	 *
	 * @param offset: specifies the offset (in bytes) into the buffer object's data store where data replacement will begin
	 * @param size:   specifies the size in bytes of the data store region being replaced
	 */
	virtual void BufferSubData(int offset, uint32_t size, const void* data) = 0;

	/**
	 * Create a new data store for a buffer object.
	 * While creating the new storage, any pre-existing data store is deleted.
	 * The new data store is created with the specified size in bytes and usage
	 *
	 * @param usage:  the symbolic constant must be GL_STREAM_DRAW, GL_STREAM_READ, GL_STREAM_COPY, GL_STATIC_DRAW,
	 *                GL_STATIC_READ, GL_STATIC_COPY, GL_DYNAMIC_DRAW, GL_DYNAMIC_READ, or GL_DYNAMIC_COPY
	 */
	virtual void BufferData(const void* data, uint64_t size, int usage) = 0;

protected:
	uint32_t _buffer;
	uint64_t _size;
};

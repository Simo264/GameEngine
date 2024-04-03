#pragma once

#include "Core/Core.hpp"

class Buffer
{
public:
	Buffer() 
		: id{ static_cast<uint32_t>(-1) }, 
			size{ 0 } 
	{}
	~Buffer() = default;

	/**
	 * Copy constructor
	 */
	Buffer(const Buffer& other) 
		: id{ other.id }, 
			size{ other.size }
	{}

	/**
	 * Assignment operator 
	 */
	Buffer& operator=(const Buffer& other) 
	{ 
		id		= other.id;
		size	= other.size;
		return *this; 
	}
	
	/**
	 * Create buffer object
	 */
	void Create();

	/**
	 * Delete buffer object
	 */
	void Delete() const;
	
	constexpr bool Compare(const Buffer& other) const { return id == other.id; }

	constexpr bool IsValid() const { return id != static_cast<uint32_t>(-1); }


	/**
	 * Create a new data store for the buffer object.
	 * While creating the new storage, any pre-existing data store is deleted.
	 * The new data store is created with the specified size in bytes and usage
	 *
	 * @param usage:  the symbolic constant must be GL_STREAM_DRAW, GL_STREAM_READ, GL_STREAM_COPY, GL_STATIC_DRAW,
	 *                GL_STATIC_READ, GL_STATIC_COPY, GL_DYNAMIC_DRAW, GL_DYNAMIC_READ, or GL_DYNAMIC_COPY
	 */
	void CreateStorage(uint64_t size, const void* data, int usage);

	/**
	 * Updates a subset of the buffer object's data store
	 *
	 * @param offset: specifies the offset (in bytes) into the buffer object's data store where data replacement will begin
	 * 
	 * @param size:   specifies the size in bytes of the data store region being replaced
	 */
	void UpdateStorage(int offset, uint32_t size, const void* data);

	/**
	 * Copy all or part of the data store of the buffer object to the data store of another buffer object
	 *
	 * @param size:       specifies the size of the data to be copied from the source buffer object
	 *                    to the destination buffer object.
	 * 
	 * @param readOffset: specifies the offset within the data store of the source buffer at which
	 *                    data will be read
	 * 
	 * @para writeOffset: specifies the offset within the data store of the destination buffer
	 *                     at which data will be written.
	 */
	void CopyStorage(const Buffer& writeBuffer, int readOffset, int writeOffset, uint64_t size);

	/**
	 * Map all of the buffer object's data store into the client's address space
	 *
	 * @param access: indicating whether it will be possible to read from, write to,
	 *								or both read from and write to the buffer object's mapped data store.
	 *								The symbolic constant must be GL_READ_ONLY, GL_WRITE_ONLY, or GL_READ_WRITE
	 */
	void* MapStorage(int access);

	/**
	 * Release the mapping of the buffer object's data store into the client's address space
	 */
	bool UnmapStorage();

	

	/**
	 * Bind the buffer object
	 */
	virtual void Bind() const = 0;

	/**
	 * Unbind the buffer object
	 */
	virtual void Unbind() const = 0;

	uint32_t id;		/* the buffer id*/
	uint64_t size;	/* the buffer size in bytes */
};

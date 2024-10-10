#pragma once

#include "Core/Core.hpp"

/**
 * https://www.khronos.org/opengl/wiki/Buffer_Object
 * 
 * Buffer Objects are OpenGL Objects that store an array of unformatted memory allocated by the OpenGL context.
 * These can be used to store vertex data, pixel data retrieved from images or the framebuffer, 
 * and a variety of other things.
 */
class Buffer
{
public:
	Buffer() : id{ 0 } {}

	/**
	 * @param target: specifies the target to which the buffer object is bound. 
	 *								See https://registry.khronos.org/OpenGL-Refpages/gl4/html/glBindBuffer.xhtml
	 */
	Buffer(u64 size, const void* data, i32 usage);
	~Buffer() = default;

	/**
	 * Create buffer object
	 */
	void Create();

	/**
	 * Delete buffer object and invalidates the name associated with the buffer object 
	 */
	void Delete();
	
	/**
	 * Create a new data store for the buffer object.
	 * While creating the new storage, any pre-existing data store is deleted.
	 * The new data store is created with the specified size in bytes and usage
	 *
	 * @param usage: the symbolic constant must be GL_STREAM_DRAW, GL_STREAM_READ, GL_STREAM_COPY, GL_STATIC_DRAW,
	 *							 GL_STATIC_READ, GL_STATIC_COPY, GL_DYNAMIC_DRAW, GL_DYNAMIC_READ, or GL_DYNAMIC_COPY
	 */
	void CreateStorage(u64 size, const void* data, i32 usage) const;

	/**
	 * Updates a subset of the buffer object's data store
	 *
	 * @param offset: specifies the offset (in bytes) into the buffer object's data store where data replacement will begin
	 * @param size:		specifies the size in bytes of the data store region being replaced
	 */
	void UpdateStorage(i32 offset, u32 size, const void* data) const;

	/**
	 * Copy all or part of the data store of the buffer object to the data store of another buffer object
	 *
	 * @param size:				specifies the size of the data to be copied from the source buffer object
	 *										to the destination buffer object.
	 * @param readOffset: specifies the offset within the data store of the source buffer at which
	 *                    data will be read
	 * @para writeOffset: specifies the offset within the data store of the destination buffer
	 *                    at which data will be written.
	 */
	void CopyStorage(const Buffer& writeBuffer, i32 readOffset, i32 writeOffset, u64 size) const;

	/**
	 * Map all of the buffer object's data store into the client's address space
	 *
	 * @param access: indicating whether it will be possible to read from, write to,
	 *								or both read from and write to the buffer object's mapped data store.
	 *								The symbolic constant must be GL_READ_ONLY, GL_WRITE_ONLY, or GL_READ_WRITE
	 */
	void* MapStorage(i32 access) const;

	/**
	 * Release the mapping of the buffer object's data store into the client's address space
	 */
	bool UnmapStorage() const;

	/**
	 * Bind the buffer object to an indexed buffer target. With glBindBufferBase target must be one of 
	 * GL_ATOMIC_COUNTER_BUFFER, GL_TRANSFORM_FEEDBACK_BUFFER, GL_UNIFORM_BUFFER or GL_SHADER_STORAGE_BUFFER.
	 * 
	 * @param target:				must be one of GL_ATOMIC_COUNTER_BUFFER, GL_TRANSFORM_FEEDBACK_BUFFER, GL_UNIFORM_BUFFER 
	 *											or GL_SHADER_STORAGE_BUFFER.
	 * @param bindingpoint: specify the index of the binding poi32 within the array specified by target.
	 */
	void BindBase(i32 target, i32 bindingpoint) const;

	/**
	 * Bind a range within the buffer object to an indexed buffer target. With glBindBufferRange target must be one of 
	 * GL_ATOMIC_COUNTER_BUFFER, GL_TRANSFORM_FEEDBACK_BUFFER, GL_UNIFORM_BUFFER, or GL_SHADER_STORAGE_BUFFER.
	 * 
	 * @param target:				must be one of GL_ATOMIC_COUNTER_BUFFER, GL_TRANSFORM_FEEDBACK_BUFFER, GL_UNIFORM_BUFFER, 
	 *											or GL_SHADER_STORAGE_BUFFER
	 * @param bindingpoint: specify the index of the binding poi32 
	 * @param offset:				the starting offset in bytes into the buffer object buffer
	 * @param size:					the amount of data in bytes that can be read from the buffer
	 */
	void BindRange(i32 target, i32 bindingpoint, i32 offset, u64 size) const;

	u32 id;
};

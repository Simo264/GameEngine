#pragma once

#include "Core/Core.hpp"

/**
 * @brief
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
	Buffer(u64 size, const void* data, i32 usage);
	~Buffer() = default;

	/**
	 * @brief
	 * Create buffer object
	 */
	void Create();

	/**
	 * @brief
	 * Delete buffer object and invalidates the name associated with the buffer object 
	 */
	void Delete();
	
	/**
	 * @brief
	 * Create a new data store for the buffer object.
	 * While creating the new storage, any pre-existing data store is deleted.
	 * The new data store is created with the specified size in bytes and usage.
	 * See https://registry.khronos.org/OpenGL-Refpages/gl4/html/glBufferData.xhtml
	 */
	void CreateStorage(u64 size, const void* data, i32 usage) const;

	/**
	 * @brief
	 * Updates a subset of the buffer object's data store.
	 * 
	 * @param offset: specifies the offset (in bytes) into the buffer object's data store where data replacement will begin
	 * @param size:		specifies the size in bytes of the data store region being replaced
	 */
	void UpdateStorage(i32 offset, u32 size, const void* data) const;

	/**
	 * @brief
	 * Copy all or part of the data store of the buffer object to the data store of another buffer object
	 *
	 * @param size:				specifies the size of the data to be copied from the source buffer object
	 *										to the destination buffer object.
	 * @param readOffset: specifies the offset within the data store of the source buffer at which
	 *                    data will be read
	 * @para writeOffset: specifies the offset within the data store of the destination buffer
	 *                    at which data will be written.
	 */
	void CopyStorage(Buffer writeBuffer, i32 readOffset, i32 writeOffset, u64 size) const;

	/**
	 * @brief
	 * Map all of the buffer object's data store into the client's address space
	 *
	 * @param access: GL_READ_ONLY, GL_WRITE_ONLY, or GL_READ_WRITE
	 */
	void* MapStorage(i32 access) const;

	/**
	 * @brief
	 * Release the mapping of the buffer object's data store into the client's address space
	 */
	bool UnmapStorage() const;

	/**
	 * @brief
	 * Bind the buffer object to an indexed buffer target. 
	 * See https://registry.khronos.org/OpenGL-Refpages/gl4/html/glBindBufferBase.xhtml
	 */
	void BindBase(i32 target, i32 bindingpoint) const;

	/**
	 * @brief
	 * Bind a range within the buffer object to an indexed buffer target. 
	 * See https://registry.khronos.org/OpenGL-Refpages/gl4/html/glBindBufferRange.xhtml
	 */
	void BindRange(i32 target, i32 bindingpoint, i32 offset, u64 size) const;

	bool IsValid() const;

	u32 id;
};

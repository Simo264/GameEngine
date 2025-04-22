#pragma once

#include "Core/Core.hpp"

enum class BufferUsage : u32
{
	STREAM_DRAW = 0x88E0, // GL_STREAM_DRAW
	STREAM_READ = 0x88E1, // GL_STREAM_READ
	STREAM_COPY = 0x88E2, // GL_STREAM_COPY
	STATIC_DRAW = 0x88E4, // GL_STATIC_DRAW
	STATIC_READ = 0x88E5, // GL_STATIC_READ
	STATIC_COPY = 0x88E6, // GL_STATIC_COPY
	DYNAMIC_DRAW = 0x88E8, // GL_DYNAMIC_DRAW
	DYNAMIC_READ = 0x88E9, // GL_DYNAMIC_READ
	DYNAMIC_COPY = 0x88EA  // GL_DYNAMIC_COPY
};

enum class BufferAccess : u32
{
	READ_ONLY = 0x88B8, // GL_READ_ONLY
	WRITE_ONLY = 0x88B9, // GL_WRITE_ONLY
	READ_WRITE = 0x88BA  // GL_READ_WRITE
};

enum class BufferTarget : u32
{
	ATOMIC_COUNTER = 0x92C0, // GL_ATOMIC_COUNTER_BUFFER
	SHADER_STORAGE = 0x90D2, // GL_SHADER_STORAGE_BUFFER
	TRANSFORM_FEEDBACK = 0x8C8E, // GL_TRANSFORM_FEEDBACK_BUFFER
	UNIFORM = 0x8A11  // GL_UNIFORM_BUFFER
};

/**
 * @brief Buffer Objects are OpenGL Objects that store an array of unformatted memory 
 * allocated by the OpenGL context.
 * These can be used to store vertex data, pixel data retrieved from images or the framebuffer, 
 * and a variety of other things.
 * 
 * https://www.khronos.org/opengl/wiki/Buffer_Object
 */
class Buffer
{
public:
	Buffer() : id{ 0 } {}
	Buffer(u64 size, const void* data, BufferUsage usage);
	~Buffer() = default;

	/** @brief Create buffer object */
	void Create();

	/** @brief Delete buffer object and invalidates the name associated with the buffer object */
	void Delete();

	/**
	 * @brief Copy all or part of the data store of a buffer object to the data store of another buffer object
	 * 
	 * @param readBuffer Specifies the name of the source buffer object
	 * @param readOffset Specifies the offset, in basic machine units, within the data store of the source buffer 
	 *										object at which data will be read.
	 * @param writeOffset Specifies the offset, in basic machine units, within the data store of the destination buffer 
	 *										object at which data will be written.
	 * @param size Specifies the size, in basic machine units, of the data to be copied from the source buffer 
	 *							object to the destination buffer object.
	 */
	void CopyFrom(u32 readBuffer, i32 readOffset, i32 writeOffset, u32 size) const;

	/**
	 * @brief Create a new data store for the buffer object.
	 * While creating the new storage, any pre-existing data store is deleted.
	 * The new data store is created with the specified size in bytes and usage.
	 * 
	 * @note If data is NULL, a data store of the specified size is still created, 
	 * but its contents remain uninitialized and thus undefined.
	 */
	void CreateStorage(u64 size, const void* data, BufferUsage usage) const;

	/**
	 * @brief Updates a subset of the buffer object's data store.
	 * 
	 * @param Offset specifies the offset (in bytes) into the buffer object's data store where data replacement will begin
	 * @param Size specifies the size in bytes of the data store region being replaced
	 * 
	 * @note If data is NULL no copy is performed and the contents of the buffer's data store become undefined.
	 */
	void UpdateStorage(i32 offset, u32 size, const void* data) const;

	/** @brief Map all of the buffer object's data store into the client's address space */
	void* MapStorage(BufferAccess access) const;

	/** @brief Release the mapping of the buffer object's data store into the client's address space */
	bool UnmapStorage() const;

	/** @brief Bind the buffer object to an indexed buffer target. */
	void BindBase(BufferTarget target, i32 bindingpoint) const;

	/** @brief Bind a range within the buffer object to an indexed buffer target. */
	void BindRange(BufferTarget target, i32 bindingpoint, i32 offset, u64 size) const;

	/** @brief Retrieves the size, in bytes, of the buffer. */
	i64 GetBufferSize() const;

	/** @brief Retrieves the OpenGL buffer usage flag. */
	i32 GetBufferUsage() const;

	bool IsValid() const;

	u32 id;
};

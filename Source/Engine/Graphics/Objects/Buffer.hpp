#pragma once

#include "Core/Core.hpp"

enum class BufferUsage : u32 
{
	StreamDraw	= 0x88E0, // GL_STREAM_DRAW
	StreamRead	= 0x88E1, // GL_STREAM_READ
	StreamCopy	= 0x88E2, // GL_STREAM_COPY
	StaticDraw	= 0x88E4, // GL_STATIC_DRAW
	StaticRead	= 0x88E5, // GL_STATIC_READ
	StaticCopy	= 0x88E6, // GL_STATIC_COPY
	DynamicDraw = 0x88E8, // GL_DYNAMIC_DRAW
	DynamicRead = 0x88E9, // GL_DYNAMIC_READ
	DynamicCopy = 0x88EA  // GL_DYNAMIC_COPY
};

enum class BufferAccess : u32 
{
	ReadOnly	= 0x88B8, // GL_READ_ONLY
	WriteOnly = 0x88B9, // GL_WRITE_ONLY
	ReadWrite = 0x88BA  // GL_READ_WRITE
};

enum class BufferTarget : u32 
{
	AtomicCounter			= 0x92C0, // GL_ATOMIC_COUNTER_BUFFER
	ShaderStorage			= 0x90D2, // GL_SHADER_STORAGE_BUFFER
	TransformFeedback = 0x8C8E, // GL_TRANSFORM_FEEDBACK_BUFFER
	Uniform						= 0x8A11  // GL_UNIFORM_BUFFER
};

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
	Buffer(u64 size, const void* data, BufferUsage usage);
	~Buffer() = default;

	/** @brief Create buffer object */
	void Create();

	/** @brief Delete buffer object and invalidates the name associated with the buffer object */
	void Delete();
	
	/**
	 * @brief
	 * Create a new data store for the buffer object.
	 * While creating the new storage, any pre-existing data store is deleted.
	 * The new data store is created with the specified size in bytes and usage.
	 */
	void CreateStorage(u64 size, const void* data, BufferUsage usage) const;

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

	/** @brief Map all of the buffer object's data store into the client's address space */
	void* MapStorage(BufferAccess access) const;

	/** @brief Release the mapping of the buffer object's data store into the client's address space */
	bool UnmapStorage() const;

	/** @brief Bind the buffer object to an indexed buffer target. */
	void BindBase(BufferTarget target, i32 bindingpoint) const;

	/** @brief Bind a range within the buffer object to an indexed buffer target. */
	void BindRange(BufferTarget target, i32 bindingpoint, i32 offset, u64 size) const;

	bool IsValid() const;

	u32 id;
};

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
enum class BufferStorageFlags : u32
{
	NONE = 0,
	DYNAMIC_STORAGE = 0x0100,  // GL_DYNAMIC_STORAGE_BIT
	MAP_READ = 0x0001,         // GL_MAP_READ_BIT
	MAP_WRITE = 0x0002,        // GL_MAP_WRITE_BIT
	MAP_PERSISTENT = 0x0040,   // GL_MAP_PERSISTENT_BIT
	MAP_COHERENT = 0x0080,     // GL_MAP_COHERENT_BIT
	CLIENT_STORAGE = 0x0200    // GL_CLIENT_STORAGE_BIT
};
enum class BufferMapFlags : u32
{
	READ = 0x0001,                    // GL_MAP_READ_BIT
	WRITE = 0x0002,                   // GL_MAP_WRITE_BIT
	INVALIDATE_RANGE = 0x0004,        // GL_MAP_INVALIDATE_RANGE_BIT
	INVALIDATE_BUFFER = 0x0008,       // GL_MAP_INVALIDATE_BUFFER_BIT
	FLUSH_EXPLICIT = 0x0010,          // GL_MAP_FLUSH_EXPLICIT_BIT
	UNSYNCHRONIZED = 0x0020,          // GL_MAP_UNSYNCHRONIZED_BIT
	PERSISTENT = 0x0040,              // GL_MAP_PERSISTENT_BIT
	COHERENT = 0x0080                 // GL_MAP_COHERENT_BIT
};

inline BufferStorageFlags operator|(BufferStorageFlags a, BufferStorageFlags b)
{
	return static_cast<BufferStorageFlags>(static_cast<u32>(a) | static_cast<u32>(b));
}
inline BufferStorageFlags operator&(BufferStorageFlags a, BufferStorageFlags b)
{
	return static_cast<BufferStorageFlags>(static_cast<u32>(a) & static_cast<u32>(b));
}
inline BufferMapFlags operator|(BufferMapFlags a, BufferMapFlags b)
{
	return static_cast<BufferMapFlags>(static_cast<u32>(a) | static_cast<u32>(b));
}

constexpr auto INVALID_BUFFER_ID = static_cast<u32>(-1);

/**
 * @brief Buffer object that stores an array of unformatted memory allocated by the OpenGL context.
 */
struct Buffer
{
	Buffer() : id{ INVALID_BUFFER_ID } {}
	/** @brief Constructs a buffer with mutable storage. */
	Buffer(u64 size, const void* data, BufferUsage usage);
	/** @brief Constructs a buffer with immutable storage. */
	Buffer(u64 size, const void* data, BufferStorageFlags flags);
	~Buffer();
	// Disable copy
	Buffer(const Buffer&) = delete;
	Buffer& operator=(const Buffer&) = delete;
	// Move semantics
	Buffer(Buffer&& other) noexcept
	{
		id = other.id;
		other.id = INVALID_BUFFER_ID;
	}
	Buffer& operator=(Buffer&& other) noexcept;

	static void DeleteBuffers(u32 count, const u32* ids);

	/** @brief Generates a new buffer ID. */
	void Create();
	/** @brief Release GPU memory. */
	void Release();
	/** @brief Creates and allocates mutable buffer storage. */
	void CreateMutableStorage(u64 size, const void* data, BufferUsage usage) const;
	/** @brief Updates a portion of the buffer's storage. */
	void UpdateStorage(i64 offset, u64 size, const void* data) const;
	/** @brief Creates and allocates immutable buffer storage. */
	void CreateImmutableStorage(u64 size, const void* data, BufferStorageFlags flags) const;
	/** @brief Maps the entire buffer storage for access. */
	template<typename T>
	T* MapStorage(BufferAccess access) const
	{
		return reinterpret_cast<T*>(__MapStorageImpl(access));
	}
	/** @brief Maps a range of the buffer's storage. */
	template<typename T>
	T* MapStorageRange(i64 offset, u64 size, BufferMapFlags flags) const
	{
		return reinterpret_cast<T*>(__MapStorageRangeImpl(offset, size, flags));
	}
	/** @brief Unmaps the buffer's storage from memory. */
	bool UnmapStorage() const;
	/** @brief Flushes a mapped range to synchronize with GPU memory. */
	void FlushMappedRange(i64 offset, u64 size) const;
	/** @brief Binds the buffer to a specific binding point. */
	void BindBase(BufferTarget target, u32 bindingpoint) const;
	/** @brief Binds a range of the buffer to a specific binding point. */
	void BindRange(BufferTarget target, u32 bindingpoint, i64 offset, u64 size) const;
	/** @brief Copies data from another buffer to this buffer. */
	void CopyFrom(u32 readBuffer, i64 readOffset, i64 writeOffset, u64 size) const;
	/** @brief Copies data from this buffer to another buffer. */
	void CopyTo(u32 writeBuffer, i64 readOffset, i64 writeOffset, u64 size) const;
	/** @brief Returns the size of the buffer in bytes. */
	i64 GetBufferSize() const;
	/** @brief Returns the current usage of the buffer. */
	BufferUsage GetBufferUsage() const;
	/** @brief Returns the current access type of the buffer. */
	BufferAccess GetBufferAccess() const;
	/** @brief Returns the storage flags of the buffer. */
	BufferStorageFlags GetStorageFlags() const;
	/** @brief Checks if the buffer is currently mapped. */
	bool IsMapped() const;
	/** @brief Returns the mapped pointer of the buffer. */
	template<typename T>
	T* GetMappedPointer() const
	{
		return reinterpret_cast<T*>(__GetMappedPointerImpl());
	}
	/** @brief Checks if the buffer has immutable storage. */
	bool IsImmutableStorage() const;
	/** @brief Clears the entire content of the buffer. */
	void Clear(u32 internalFormat, u32 format, u32 type, const void* data) const;
	/** @brief Clears a sub-range of the buffer's data. */
	void ClearSubData(u32 internalFormat, i64 offset, u64 size, u32 format, u32 type, const void* data) const;

	auto IsValid() const { return id != INVALID_BUFFER_ID; }

	u32 id;
private:
	void* __MapStorageImpl(BufferAccess access) const;
	void* __MapStorageRangeImpl(i64 offset, u64 size, BufferMapFlags flags) const;
	void* __GetMappedPointerImpl() const;
};

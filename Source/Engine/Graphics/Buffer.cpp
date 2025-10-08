#include "Buffer.hpp"

#include <glad/gl.h>

Buffer::Buffer(u64 size, const void* data, BufferUsage usage)
{
	Create();
	CreateMutableStorage(size, data, usage);
}

Buffer::Buffer(u64 size, const void* data, BufferStorageFlags flags)
{
	Create();
	CreateImmutableStorage(size, data, flags);
}

Buffer& Buffer::operator=(Buffer&& other) noexcept
{
	if (this != &other)
	{
		Release();

		id = other.id;
		other.id = INVALID_BUFFER_ID;
	}
	return *this;
}

Buffer::~Buffer()
{
	Release();
}

void Buffer::DeleteBuffers(u32 count, const BufferId* ids)
{
	glDeleteBuffers(count, ids);
}

void Buffer::Create()
{
	glCreateBuffers(1, &id);
}

void Buffer::Release()
{
	if (Valid())
	{
		glDeleteBuffers(1, &id);
		id = INVALID_BUFFER_ID;
	}
}

void Buffer::CreateMutableStorage(u64 size, const void* data, BufferUsage usage) const
{
	glNamedBufferData(id, size, data, static_cast<u32>(usage));
}

void Buffer::UpdateStorage(i64 offset, u64 size, const void* data) const
{
	glNamedBufferSubData(id, offset, size, data);
}

void Buffer::CreateImmutableStorage(u64 size, const void* data, BufferStorageFlags flags) const
{
	glNamedBufferStorage(id, size, data, static_cast<u32>(flags));
}

void* Buffer::__MapStorageImpl(BufferAccess access) const
{
	return glMapNamedBuffer(id, static_cast<u32>(access));
}

void* Buffer::__MapStorageRangeImpl(i64 offset, u64 size, BufferMapFlags flags) const
{
	return glMapNamedBufferRange(id, offset, size, static_cast<u32>(flags));
}

bool Buffer::UnmapStorage() const
{
	return glUnmapNamedBuffer(id) == GL_TRUE;
}

void Buffer::FlushMappedRange(i64 offset, u64 size) const
{
	glFlushMappedNamedBufferRange(id, offset, size);
}

void Buffer::BindBase(BufferTarget target, u32 bindingpoint) const
{
	glBindBufferBase(static_cast<u32>(target), bindingpoint, id);
}

void Buffer::BindRange(BufferTarget target, u32 bindingpoint, i64 offset, u64 size) const
{
	glBindBufferRange(static_cast<u32>(target), bindingpoint, id, offset, size);
}

void Buffer::CopyFrom(BufferId readBuffer, i64 readOffset, i64 writeOffset, u64 size) const
{
	glCopyNamedBufferSubData(readBuffer, id, readOffset, writeOffset, size);
}

void Buffer::CopyTo(BufferId writeBuffer, i64 readOffset, i64 writeOffset, u64 size) const
{
	glCopyNamedBufferSubData(id, writeBuffer, readOffset, writeOffset, size);
}

i64 Buffer::GetBufferSize() const
{
	auto size = static_cast<i64>(0);
	glGetNamedBufferParameteri64v(id, GL_BUFFER_SIZE, &size);
	return size;
}

BufferUsage Buffer::GetBufferUsage() const
{
	auto usage = 0;
	glGetNamedBufferParameteriv(id, GL_BUFFER_USAGE, &usage);
	return static_cast<BufferUsage>(usage);
}

BufferAccess Buffer::GetBufferAccess() const
{
	auto access = 0;
	glGetNamedBufferParameteriv(id, GL_BUFFER_ACCESS, &access);
	return static_cast<BufferAccess>(access);
}

bool Buffer::IsMapped() const
{
	auto mapped = 0;
	glGetNamedBufferParameteriv(id, GL_BUFFER_MAPPED, &mapped);
	return mapped == GL_TRUE;
}

void* Buffer::__GetMappedPointerImpl() const
{
	void* pointer = nullptr;
	glGetNamedBufferPointerv(id, GL_BUFFER_MAP_POINTER, &pointer);
	return pointer;
}

bool Buffer::IsImmutableStorage() const
{
	auto immutable = 0;
	glGetNamedBufferParameteriv(id, GL_BUFFER_IMMUTABLE_STORAGE, &immutable);
	return immutable == GL_TRUE;
}

BufferStorageFlags Buffer::GetStorageFlags() const
{
	auto flags = 0;
	glGetNamedBufferParameteriv(id, GL_BUFFER_STORAGE_FLAGS, &flags);
	return static_cast<BufferStorageFlags>(flags);
}

void Buffer::Clear(u32 internalFormat, u32 format, u32 type, const void* data) const
{
	glClearNamedBufferData(id, internalFormat, format, type, data);
}

void Buffer::ClearSubData(u32 internalFormat, i64 offset, u64 size, u32 format, u32 type, const void* data) const
{
	glClearNamedBufferSubData(id, internalFormat, offset, size, format, type, data);
}



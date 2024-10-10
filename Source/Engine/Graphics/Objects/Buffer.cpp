#include "Buffer.hpp"

#include "Core/GL.hpp"
#include "Core/Log/Logger.hpp"

Buffer::Buffer(u64 size, const void* data, i32 usage)
{
	Create();
	CreateStorage(size, data, usage);
}

void Buffer::Create()
{
	glCreateBuffers(1, &id);
}

void Buffer::Delete()
{
	glDeleteBuffers(1, &id);
	id = 0;
}

void Buffer::CopyStorage(const Buffer& writeBuffer, i32 readOffset, i32 writeOffset, u64 size) const
{
	glCopyNamedBufferSubData(id, writeBuffer.id, readOffset, writeOffset, size);
}

void Buffer::CreateStorage(u64 size, const void* data, i32 usage) const
{
	glNamedBufferData(id, size, data, usage);
}

void Buffer::UpdateStorage(i32 offset, u32 size, const void* data) const
{
	glNamedBufferSubData(id, offset, size, data);
}

void* Buffer::MapStorage(i32 access) const
{
	return glMapNamedBuffer(id, access);
}

bool Buffer::UnmapStorage() const
{
	return glUnmapNamedBuffer(id);
}

void Buffer::BindBase(i32 target, i32 bindingpoint) const
{
	glBindBufferBase(target, bindingpoint, id);
}

void Buffer::BindRange(i32 target, i32 bindingpoint, i32 offset, u64 size) const
{
	glBindBufferRange(target, bindingpoint, id, offset, size);
}

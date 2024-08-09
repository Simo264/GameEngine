#include "Buffer.hpp"

#include "Core/OpenGL.hpp"
#include "Core/Log/Logger.hpp"

Buffer::Buffer()
	: id{ 0 },
		target{ 0 }
{}

Buffer::Buffer(int target, uint64_t size, const float* data, int usage)
	: target{ target }
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

void Buffer::CopyStorage(const Buffer& writeBuffer, int readOffset, int writeOffset, uint64_t size) const
{
	glCopyNamedBufferSubData(id, writeBuffer.id, readOffset, writeOffset, size);
}

void Buffer::CreateStorage(uint64_t size, const void* data, int usage) const
{
	glNamedBufferData(id, size, data, usage);
}

void Buffer::UpdateStorage(int offset, uint32_t size, const void* data) const
{
	glNamedBufferSubData(id, offset, size, data);
}

void* Buffer::MapStorage(int access) const
{
	return glMapNamedBuffer(id, access);
}

bool Buffer::UnmapStorage() const
{
	return glUnmapNamedBuffer(id);
}

void Buffer::Bind() const
{
	glBindBuffer(target, id);
}

void Buffer::BindBase(int bindingpoint) const
{
	glBindBufferBase(target, bindingpoint, id);
}

void Buffer::BindRange(int bindingpoint, int offset, uint64_t size) const
{
	glBindBufferRange(target, bindingpoint, id, offset, size);
}

void Buffer::Unbind() const
{
	glBindBuffer(target, 0);
}

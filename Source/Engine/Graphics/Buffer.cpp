#include "Buffer.hpp"

#include "Core/OpenGL.hpp"
#include "Core/Log/Logger.hpp"

Buffer::Buffer()
	: id{ static_cast<uint32_t>(-1) },
		target{ -1 },
		size{ 0 }
{}

Buffer::Buffer(int target, uint64_t size, const uint32_t* data, int usage)
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
	id = static_cast<uint32_t>(-1);
}

void Buffer::CopyStorage(const Buffer& writeBuffer, int readOffset, int writeOffset, uint64_t size) const
{
	glCopyNamedBufferSubData(id, writeBuffer.id, readOffset, writeOffset, size);
}

void Buffer::CreateStorage(uint64_t size, const void* data, int usage)
{
	this->size = size;
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
	if (target == -1)
		CONSOLE_WARN("Invalid buffer target!");

	glBindBuffer(target, id);
}

void Buffer::Unbind() const
{
	if (target == -1)
		CONSOLE_WARN("Invalid buffer target!");

	glBindBuffer(target, 0);
}

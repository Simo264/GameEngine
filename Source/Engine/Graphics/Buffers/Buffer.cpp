#include "Buffer.hpp"

#include "Core/Platform/OpenGL/OpenGL.hpp"

void Buffer::Create()
{
	glCreateBuffers(1, &id);
}

void Buffer::Delete() const
{
	glDeleteBuffers(1, &id);
}

void Buffer::CopyStorage(const Buffer& writeBuffer, int readOffset, int writeOffset, uint64_t size)
{
	glCopyNamedBufferSubData(id, writeBuffer.id, readOffset, writeOffset, size);
}

void Buffer::CreateStorage(uint64_t size, const void* data, int usage)
{
	this->size = size;
	glNamedBufferData(id, size, data, usage);
}

void Buffer::UpdateStorage(int offset, uint32_t size, const void* data)
{
	glNamedBufferSubData(id, offset, size, data);
}

void* Buffer::MapStorage(int access)
{
	return glMapNamedBuffer(id, access);
}

bool Buffer::UnmapStorage()
{
	return glUnmapNamedBuffer(id);
}
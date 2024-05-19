#include "Buffer.hpp"

#include "Core/OpenGL.hpp"

Buffer::Buffer()
	: id{ static_cast<uint32_t>(-1) },
		_size{ 0 }
{}

void Buffer::Create()
{
	glCreateBuffers(1, &id);
}

void Buffer::Delete()
{
	glDeleteBuffers(1, &id);
	
	id = static_cast<uint32_t>(-1);
}

void Buffer::CopyStorage(const Buffer& writeBuffer, int readOffset, int writeOffset, uint64_t size)
{
	glCopyNamedBufferSubData(id, writeBuffer.id, readOffset, writeOffset, size);
}

void Buffer::CreateStorage(uint64_t size, const void* data, int usage)
{
	_size = size;
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

bool Buffer::IsBuffer(uint32_t buffer)
{
	return glIsBuffer(buffer);
}

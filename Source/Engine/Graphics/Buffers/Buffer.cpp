#include "Buffer.hpp"

#include "Core/Platform/OpenGL/OpenGL.hpp"

void Buffer::Create()
{
	glCreateBuffers(1, &bufferID);
}

void Buffer::Delete() const
{
	glDeleteBuffers(1, &bufferID);
}

void Buffer::CopyStorage(const Buffer& writeBuffer, int readOffset, int writeOffset, uint64_t size)
{
	glCopyNamedBufferSubData(bufferID, writeBuffer.bufferID, readOffset, writeOffset, size);
}

void Buffer::CreateStorage(uint64_t size, const void* data, int usage)
{
	glNamedBufferData(bufferID, size, data, usage);
}

void Buffer::UpdateStorage(int offset, uint32_t size, const void* data)
{
	glNamedBufferSubData(bufferID, offset, size, data);
}

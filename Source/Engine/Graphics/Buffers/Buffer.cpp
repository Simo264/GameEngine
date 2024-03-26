#include "Buffer.hpp"

#include "Core/Platform/OpenGL/OpenGL.hpp"

void Buffer::Generate()
{
	glGenBuffers(1, &_buffer);
}

void Buffer::Delete() const
{
	glDeleteBuffers(1, &_buffer);
}

void Buffer::Copy(const Buffer& other, uint64_t size, int readOffset, int writeOffset)
{
	glBindBuffer(GL_COPY_READ_BUFFER, _buffer);
	glBindBuffer(GL_COPY_WRITE_BUFFER, other.GetBufferID());
	glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, readOffset, writeOffset, size);
	glBindBuffer(GL_COPY_READ_BUFFER, 0);
	glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
}
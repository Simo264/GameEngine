#include "ElementBuffer.hpp"

#include "Core/Platform/OpenGL/OpenGL.hpp"

ElementBuffer::ElementBuffer(const void* data, uint64_t size, int usage)
{
	Generate();

	Bind();
	BufferData(data, size, usage);
	Unbind();
}

void ElementBuffer::Generate()
{
	glGenBuffers(1, &_buffer);
}

void ElementBuffer::BufferData(const void* data, uint64_t size, int usage)
{
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, usage);
}

void ElementBuffer::BufferSubData(int offset, uint32_t size, const void* data)
{
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, size, data);
}

void ElementBuffer::Bind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _buffer);
}

void ElementBuffer::Unbind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void ElementBuffer::DeleteBuffer() const
{
	glDeleteBuffers(1, &_buffer);
}

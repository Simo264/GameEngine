#include "VertexBuffer.hpp"

#include "Core/Platform/OpenGL/OpenGL.hpp"

VertexBuffer::VertexBuffer(const void* data, uint64_t size, int usage)
{
	Generate();

	Bind();
		BufferData(data, size, usage);
	Unbind();
}

void VertexBuffer::Bind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, _buffer);
}

void VertexBuffer::Unbind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBuffer::BufferData(const void* data, uint64_t size, int usage)
{
	_size = size;
	glBufferData(GL_ARRAY_BUFFER, size, data, usage);
	//glNamedBufferData(_buffer, size, data, usage);
}

void VertexBuffer::BufferSubData(int offset, uint32_t size, const void* data)
{
	glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
	//glNamedBufferData(_buffer, size, data, usage);
}


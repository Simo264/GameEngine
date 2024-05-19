#include "VertexBuffer.hpp"

#include "Core/OpenGL.hpp"

VertexBuffer::VertexBuffer()
	: Buffer()
{}

VertexBuffer::VertexBuffer(uint64_t size, const void* data, int usage)
	: Buffer()
{
	Create();
	CreateStorage(size, data, usage);
}

void VertexBuffer::Bind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, id);
}

void VertexBuffer::Unbind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

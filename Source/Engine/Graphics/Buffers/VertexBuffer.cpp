#include "VertexBuffer.hpp"

#include "Core/Platform/OpenGL/OpenGL.hpp"

VertexBuffer::VertexBuffer(uint64_t size, const void* data, int usage)
	: Buffer(), 
		numVertices{ 0 }
{
	Create();
	CreateStorage(size, data, usage);
}

void VertexBuffer::Bind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, bufferID);
}

void VertexBuffer::Unbind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

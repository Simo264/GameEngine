#include "ElementBuffer.hpp"

#include "Core/Platform/OpenGL/OpenGL.hpp"

ElementBuffer::ElementBuffer()
	: Buffer()
{}

ElementBuffer::ElementBuffer(uint64_t size, const uint32_t* data, int usage)
	: Buffer()
{
	Create();
	CreateStorage(size, data, usage);
}

void ElementBuffer::Bind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
}

void ElementBuffer::Unbind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
}


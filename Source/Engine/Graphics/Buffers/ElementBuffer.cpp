#include "ElementBuffer.hpp"

#include "Core/Platform/OpenGL/OpenGL.hpp"

ElementBuffer::ElementBuffer(uint64_t size, const void* data, int usage)
	: Buffer()
{
	Create();
	CreateStorage(size, data, usage);
	numIndices = size / sizeof(ElementBufferDataType);
}

void ElementBuffer::Bind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferID);
}

void ElementBuffer::Unbind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferID);
}


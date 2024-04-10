#include "RenderBuffer.hpp"

#include "Core/Platform/OpenGL/OpenGL.hpp"

RenderBuffer::RenderBuffer()
	: id{ static_cast<uint32_t>(-1) }
{}

void RenderBuffer::Create()
{
	glCreateRenderbuffers(1, &id);
}

void RenderBuffer::Delete()
{
	glDeleteRenderbuffers(1, &id);
	
	id = static_cast<uint32_t>(-1);
}

void RenderBuffer::CreateStorage(int internalformat, int width, int height) const
{
	glNamedRenderbufferStorage(id, internalformat, width, height);
}

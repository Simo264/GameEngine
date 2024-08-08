#include "RenderBuffer.hpp"

#include "Core/OpenGL.hpp"
#include "Core/Log/Logger.hpp"

RenderBuffer::RenderBuffer()
	: id{ static_cast<uint32_t>(-1) },
	internalFormat{ GL_RGB8 }
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

void RenderBuffer::CreateStorage(int internalFormat, int width, int height)
{
	this->internalFormat = internalFormat;
	glNamedRenderbufferStorage(id, internalFormat, width, height);
}

void RenderBuffer::CreateStorageMulstisampled(int internalFormat, int samples, int width, int height)
{
	this->internalFormat = internalFormat;
	glNamedRenderbufferStorageMultisample(id, samples, internalFormat, width, height);
}

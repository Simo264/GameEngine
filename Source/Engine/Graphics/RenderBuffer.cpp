#include "RenderBuffer.hpp"

#include "Core/Platform/OpenGL/OpenGL.hpp"
#include "Core/Log/Logger.hpp"

RenderBuffer::RenderBuffer()
	: id{ static_cast<uint32_t>(-1) },
		internalformat{ 0 }
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

void RenderBuffer::CreateStorage(int width, int height) const
{
	if(internalformat == 0)
		CONSOLE_WARN("Invalid renderbuffer internalformat");

	glNamedRenderbufferStorage(id, internalformat, width, height);
}

void RenderBuffer::CreateStorageMulstisampled(int samples, int width, int height) const
{
	if (internalformat == 0)
		CONSOLE_WARN("Invalid renderbuffer internalformat");

	glNamedRenderbufferStorageMultisample(id, samples, internalformat, width, height);
}

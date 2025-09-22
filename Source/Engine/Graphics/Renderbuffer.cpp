#include "RenderBuffer.hpp"
#include <glad/gl.h>

void Renderbuffer::Create()
{
	glCreateRenderbuffers(1, &id);
}

void Renderbuffer::Release()
{
	glDeleteRenderbuffers(1, &id);
	id = INVALID_RENDERBUFFER_ID;
}

void Renderbuffer::CreateStorage(RenderbufferInternalFormat internalFormat, i32 width, i32 height) const
{
	glNamedRenderbufferStorage(id, static_cast<u32>(internalFormat), width, height);
}

void Renderbuffer::CreateStorageMulstisampled(RenderbufferInternalFormat internalFormat, i32 samples, i32 width, i32 height) const
{
	glNamedRenderbufferStorageMultisample(id, samples, static_cast<u32>(internalFormat), width, height);
}

#include "RenderBuffer.hpp"

#include "Core/GL.hpp"
#include "Core/Log/Logger.hpp"

void RenderBuffer::Create()
{
	glCreateRenderbuffers(1, &id);
}

void RenderBuffer::Delete()
{
	glDeleteRenderbuffers(1, &id);
	id = 0;
}

bool RenderBuffer::IsValid() const
{
	return (id != 0) && (glIsRenderbuffer(id) == GL_TRUE);
}

void RenderBuffer::CreateStorage(i32 internalFormat, i32 width, i32 height) const
{
	glNamedRenderbufferStorage(id, internalFormat, width, height);
}

void RenderBuffer::CreateStorageMulstisampled(i32 internalFormat, i32 samples, i32 width, i32 height) const
{
	glNamedRenderbufferStorageMultisample(id, samples, internalFormat, width, height);
}

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

void RenderBuffer::CreateStorage(RenderbufferInternalFormat internalFormat, i32 width, i32 height) const
{
	glNamedRenderbufferStorage(
		id, 
		static_cast<u32>(internalFormat),
		width, 
		height);
}

void RenderBuffer::CreateStorageMulstisampled(RenderbufferInternalFormat internalFormat, i32 samples, i32 width, i32 height) const
{
	glNamedRenderbufferStorageMultisample(
		id, 
		samples, 
		static_cast<u32>(internalFormat),
		width, 
		height);
}

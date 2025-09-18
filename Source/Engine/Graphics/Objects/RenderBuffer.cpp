#include "RenderBuffer.hpp"

#include "Core/OpenGL.hpp"
#include "Core/Log/Logger.hpp"

void Renderbuffer::Create()
{
	glCreateRenderbuffers(1, &id);
}

void Renderbuffer::Delete()
{
	glDeleteRenderbuffers(1, &id);
	id = 0u;
}

bool Renderbuffer::IsValid() const
{
	return (id != 0) && (glIsRenderbuffer(id) == GL_TRUE);
}

void Renderbuffer::CreateStorage(RenderbufferInternalFormat internalFormat, i32 width, i32 height) const
{
	glNamedRenderbufferStorage(id, 
														 static_cast<u32>(internalFormat),
														 width, 
														 height);
}

void Renderbuffer::CreateStorageMulstisampled(RenderbufferInternalFormat internalFormat, i32 samples, i32 width, i32 height) const
{
	glNamedRenderbufferStorageMultisample(id, 
																				samples, 
																				static_cast<u32>(internalFormat),
																				width, 
																				height);
}

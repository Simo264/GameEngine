#include "FrameBuffer.hpp"

#include "Core/Log/Logger.hpp"

#include "Core/Platform/OpenGL/OpenGL.hpp"

/* -----------------------------------------------------
 *          PUBLIC METHODS
 * -----------------------------------------------------
*/

FrameBuffer::FrameBuffer(const Vec2i& size)
	: _size{ size },
		_samples{ 4 } /* default samples value */
{
	_fbo																= static_cast<uint32_t>(0xFFFFFFFF);
	_intermediateFbo										= static_cast<uint32_t>(0xFFFFFFFF);
	_colorAttachment										= static_cast<uint32_t>(0xFFFFFFFF);
	_colorAttachmentMultisampled				= static_cast<uint32_t>(0xFFFFFFFF);
	_depthStencilAttachmentMultisampled = static_cast<uint32_t>(0xFFFFFFFF);

	/* Create framebuffer objects */
	glGenFramebuffers(1, &_fbo);
	glGenFramebuffers(1, &_intermediateFbo);

	/* Create texture objects */
	glGenTextures(1, &_colorAttachment);
	glGenTextures(1, &_colorAttachmentMultisampled);
	
	/* Create renderbuffer objects */
	glGenRenderbuffers(1, &_depthStencilAttachmentMultisampled);
}

FrameBuffer::~FrameBuffer()
{
	/* Delete framebuffer objects */
	glDeleteFramebuffers(1, &_fbo);
	glDeleteFramebuffers(1, &_intermediateFbo);
	
	/* Delete texture objects */
	glDeleteTextures(1, &_colorAttachment);
	glDeleteTextures(1, &_colorAttachmentMultisampled);
	
	/* Delete renderbuffer objects */
	glDeleteRenderbuffers(1, &_depthStencilAttachmentMultisampled);
}

void FrameBuffer::AttachMultisampledColorBuffers()
{
	/* Create a multisampled color attachment texture */
	glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, _colorAttachmentMultisampled);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, _samples, GL_RGB, _size.x, _size.y, GL_TRUE);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, _colorAttachmentMultisampled, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	/* Create a color attachment texture */
	glBindFramebuffer(GL_FRAMEBUFFER, _intermediateFbo);
	glBindTexture(GL_TEXTURE_2D, _colorAttachment);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _size.x, _size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _colorAttachment, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void FrameBuffer::AttachMultisampledDepthStencilBuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, _fbo); 
	
	/* Create a multisampled renderbuffer object for depth and stencil attachments */
	glBindRenderbuffer(GL_RENDERBUFFER, _depthStencilAttachmentMultisampled);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, _samples, GL_DEPTH24_STENCIL8, _size.x, _size.y);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _depthStencilAttachmentMultisampled);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::Rescale(const Vec2i& size)
{
	_size = size;

	/* Resize textures */
	UpdateMultisampledTexture(_colorAttachmentMultisampled, _samples, _size.x, _size.y);
	UpdateTexture(_colorAttachment, _size.x, _size.y);

	/* Resize renderbuffers */
	UpdateMultisampledRenderbuffer(_depthStencilAttachmentMultisampled, _samples, _size.x, _size.y);
}

void FrameBuffer::Blit() const
{
	/* Blit multisampled buffer to normal color buffer of intermediate FBO. Image is stored in _colorAttachment */
	glBindFramebuffer(GL_READ_FRAMEBUFFER, _fbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _intermediateFbo);
	glBlitFramebuffer(0, 0, _size.x, _size.y, 0, 0, _size.x, _size.y, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void FrameBuffer::SetSamples(int samples)
{
	if (samples != 1 && samples != 2 && samples != 4 && samples != 8)
	{
		CONSOLE_WARN("Invalid samples value");
		return;
	}

	_samples = samples;

	/* Update multisampled texture objects */
	UpdateMultisampledTexture(_colorAttachmentMultisampled, _samples, _size.x, _size.y);

	/* Update multisampled renderbuffer objects */
	UpdateMultisampledRenderbuffer(_depthStencilAttachmentMultisampled, _samples, _size.x, _size.y);
}

bool FrameBuffer::CheckStatus() const { return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE; }
void FrameBuffer::Bind() const { glBindFramebuffer(GL_FRAMEBUFFER, _fbo); }
void FrameBuffer::Unbind() const { glBindFramebuffer(GL_FRAMEBUFFER, 0); }
void FrameBuffer::BindRead() const { glBindFramebuffer(GL_READ_FRAMEBUFFER, _fbo); }
void FrameBuffer::UnbindRead() const { glBindFramebuffer(GL_READ_FRAMEBUFFER, 0); }
void FrameBuffer::BindWrite()	const { glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fbo); }
void FrameBuffer::UnbindWrite() const { glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); }

/* -----------------------------------------------------
 *          PRIVATE METHODS
 * -----------------------------------------------------
*/

void FrameBuffer::UpdateTexture(uint32_t texture, int width, int height)
{
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);
}
void FrameBuffer::UpdateMultisampledTexture(uint32_t texture, int samples, int width, int height)
{
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texture);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGB, width, height, GL_TRUE);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
}

void FrameBuffer::UpdateMultisampledRenderbuffer(uint32_t renderbuffer, int samples, int width, int height)
{
	glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH24_STENCIL8, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

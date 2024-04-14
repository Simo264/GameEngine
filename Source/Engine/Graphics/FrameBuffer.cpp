#include "FrameBuffer.hpp"

#include "Core/Platform/OpenGL/OpenGL.hpp"
#include "Core/Log/Logger.hpp"

/* -----------------------------------------------------
 *          PUBLIC METHODS
 * -----------------------------------------------------
*/

FrameBuffer::FrameBuffer()
	: id{ static_cast<uint32_t>(-1) },
		_textAttachments{},
		_rboAttachments{}
{}

void FrameBuffer::Create()
{
	glCreateFramebuffers(1, &id);
}

void FrameBuffer::Delete()
{
	glDeleteFramebuffers(1, &id);

	for (Texture2D& texture : _textAttachments)
		texture.Delete();

	for (RenderBuffer& rbo : _rboAttachments)
		rbo.Delete();

	id = static_cast<uint32_t>(-1);
	_textAttachments.clear();
	_rboAttachments.clear();
}

void FrameBuffer::Bind(int target) const
{
	glBindFramebuffer(target, id);
}

void FrameBuffer::Unbind(int target) const
{
	glBindFramebuffer(target, 0);
}

int FrameBuffer::CheckStatus() const
{
	return glCheckNamedFramebufferStatus(id, GL_FRAMEBUFFER);
}

void FrameBuffer::AttachTexture(int attachment, const Texture2D& texture, int level)
{
	glNamedFramebufferTexture(id, attachment, texture.id, level);

	_textAttachments.push_back(texture);
}

void FrameBuffer::AttachRenderBuffer(int attachment, const RenderBuffer& renderbuffer)
{
	glNamedFramebufferRenderbuffer(id, attachment, GL_RENDERBUFFER, renderbuffer.id);
	
	_rboAttachments.push_back(renderbuffer);
}

void FrameBuffer::Blit(
	const FrameBuffer& dest,
	int srcLowerX,
	int srcLowerY,
	int srcUpperX,
	int srcUpperY,
	int destLowerX,
	int destLowerY,
	int destUpperX,
	int destUpperY,
	int mask,
	int filter) const
{
	glBlitNamedFramebuffer(
		id, dest.id, srcLowerX, srcLowerY, srcUpperX, srcUpperY, destLowerX, destLowerY, destUpperX, destUpperY, mask, filter);
}

#if 0
FrameBuffer::FrameBuffer(const Vec2i32& size)
	: _size{ size },
		_samples{ 1 } /* default samples value */
{
	/* Create framebuffer objects */
	glGenFramebuffers(1, &_fbo);
	glGenFramebuffers(1, &_intermediateFbo);

	/* Create texture objects */
	glGenTextures(1, &_colorAttachment);
	glGenTextures(1, &_colorAttachmentMultisampled);
	
	/* Create renderbuffer objects */
	glGenRenderbuffers(1, &_depthStencilAttachmentMultisampled);

	ColorAttachment();

	DepthStencilAttachment();

	assert(CheckStatus() && "Framebuffer is not complete!");
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

void FrameBuffer::Rescale(const Vec2i32& size)
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
	glBindFramebuffer(GL_READ_FRAMEBUFFER, _fbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _intermediateFbo);
	glBlitFramebuffer(0, 0, _size.x, _size.y, 0, 0, _size.x, _size.y, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}
void FrameBuffer::SetSamples(int samples)
{
	if (samples <= 0)
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

void FrameBuffer::Bind() const { glBindFramebuffer(GL_FRAMEBUFFER, _fbo); }
void FrameBuffer::Unbind() const { glBindFramebuffer(GL_FRAMEBUFFER, 0); }
void FrameBuffer::BindRead() const { glBindFramebuffer(GL_READ_FRAMEBUFFER, _fbo); }
void FrameBuffer::UnbindRead() const { glBindFramebuffer(GL_READ_FRAMEBUFFER, 0); }
void FrameBuffer::BindWrite()	const { glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fbo); }
void FrameBuffer::UnbindWrite() const { glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); }
#endif

/* -----------------------------------------------------
 *          PRIVATE METHODS
 * -----------------------------------------------------
*/

#if 0
void FrameBuffer::ColorAttachment()
{
	/* Normal color attachment */
	glBindFramebuffer(GL_FRAMEBUFFER, _intermediateFbo);
	glBindTexture(GL_TEXTURE_2D, _colorAttachment);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _size.x, _size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _colorAttachment, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	/* Multisampled color attachment */
	glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, _colorAttachmentMultisampled);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, _samples, GL_RGB, _size.x, _size.y, GL_TRUE);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, _colorAttachmentMultisampled, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void FrameBuffer::DepthStencilAttachment()
{
	glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
	glBindRenderbuffer(GL_RENDERBUFFER, _depthStencilAttachmentMultisampled);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, _samples, GL_DEPTH24_STENCIL8, _size.x, _size.y);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _depthStencilAttachmentMultisampled);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

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

bool FrameBuffer::CheckStatus() const { return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE; }
#endif

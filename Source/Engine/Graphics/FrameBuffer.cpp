#include "FrameBuffer.hpp"

#include "Core/Log/Logger.hpp"

#include "Core/Platform/OpenGL/OpenGL.hpp"

/* -----------------------------------------------------
 *          PUBLIC METHODS
 * -----------------------------------------------------
*/

FrameBuffer::FrameBuffer(const Vec2i& size)
	: _size{ size },
		_fbo{ 0 },
		_textureColorBuffer{ static_cast<uint32_t>(0xFFFFFFFF) },		/* Set id to infinity*/
		_textureDepthBuffer{ static_cast<uint32_t>(0xFFFFFFFF) },		/* Set id to infinity*/
		_textureStencilBuffer{ static_cast<uint32_t>(0xFFFFFFFF) }	/* Set id to infinity*/
{
	/* Creating a framebuffer object */
	glGenFramebuffers(1, &_fbo);

	/* 1. We have to attach at least one buffer (color, depth or stencil buffer).
	   2. There should be at least one color attachment.
	   3. All attachments should be complete as well (reserved memory).
	   4. Each buffer should have the same number of samples. */

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

FrameBuffer::~FrameBuffer()
{
	glDeleteFramebuffers(1, &_fbo);
	
	if (_textureColorBuffer != static_cast<uint32_t>(0xFFFFFFFF))
		glDeleteTextures(1, &_textureColorBuffer);
	
	if (_textureDepthBuffer != static_cast<uint32_t>(0xFFFFFFFF))
		glDeleteTextures(1, &_textureDepthBuffer);
	
	if (_textureStencilBuffer != static_cast<uint32_t>(0xFFFFFFFF))
		glDeleteTextures(1, &_textureStencilBuffer);
}

void FrameBuffer::Bind() const { glBindFramebuffer(GL_FRAMEBUFFER, _fbo); }
void FrameBuffer::Unbind() const { glBindFramebuffer(GL_FRAMEBUFFER, 0); }
void FrameBuffer::BindRead() const { glBindFramebuffer(GL_READ_FRAMEBUFFER, _fbo); }
void FrameBuffer::UnbindRead() const { glBindFramebuffer(GL_READ_FRAMEBUFFER, 0); }
void FrameBuffer::BindWrite()	const	{ glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fbo); }
void FrameBuffer::UnbindWrite() const { glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); }


void FrameBuffer::AttachColorBuffer()
{
	if (_textureColorBuffer != static_cast<uint32_t>(0xFFFFFFFF))
	{
		CONSOLE_WARN("Depth buffer is already attached!");
		return;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

	glGenTextures(1, &_textureColorBuffer);
	glBindTexture(GL_TEXTURE_2D, _textureColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _size.x, _size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _textureColorBuffer, 0);
}
void FrameBuffer::AttachDepthBuffer()
{
	if (_textureDepthBuffer != static_cast<uint32_t>(0xFFFFFFFF))
	{
		CONSOLE_WARN("Depth buffer is already attached!");
		return;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

	glGenTextures(1, &_textureDepthBuffer);
	glBindTexture(GL_TEXTURE_2D, _textureDepthBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, _size.x, _size.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _textureDepthBuffer, 0);
}
void FrameBuffer::AttachStencilBuffer()
{
	if (_textureStencilBuffer != static_cast<uint32_t>(0xFFFFFFFF))
	{
		CONSOLE_WARN("Depth buffer is already attached!");
		return;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
	glGenTextures(1, &_textureStencilBuffer);
	glBindTexture(GL_TEXTURE_2D, _textureStencilBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_STENCIL_INDEX8, _size.x, _size.y, 0, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, _textureStencilBuffer, 0);
}

bool FrameBuffer::CheckStatus() const
{
	return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
}

void FrameBuffer::Rescale(Vec2i size)
{
	_size = size;

	/* Resize color buffer if exists */
	if (_textureColorBuffer != static_cast<uint32_t>(0xFFFFFFFF))
	{
		glBindTexture(GL_TEXTURE_2D, _textureColorBuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _size.x, _size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	}
	/* Resize depth buffer if exists */
	if (_textureDepthBuffer != static_cast<uint32_t>(0xFFFFFFFF))
	{
		glBindTexture(GL_TEXTURE_2D, _textureDepthBuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, _size.x, _size.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	}
	/* Resize stecil buffer if exists */
	if (_textureStencilBuffer != static_cast<uint32_t>(0xFFFFFFFF))
	{
		glBindTexture(GL_TEXTURE_2D, _textureStencilBuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_STENCIL_INDEX8, _size.x, _size.y, 0, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE, nullptr);
	}

}

#if 0
FrameBuffer::FrameBuffer(int w, int h)
{
	_renderRelatedIds[NORMAL_FBO] = 0;
	_renderRelatedIds[NORMAL_TEXTURE] = 0;
	_renderRelatedIds[MULTISAMPLING_FBO] = 0;
	_renderRelatedIds[MULTISAMPLING_TEXTURE] = 0;
	_renderRelatedIds[MULTISAMPLING_RBO] = 0;
	
	_postprocType = PostProcessingType::POST_PROC_NONE;
	_size = Vec2i(w, h);

	InitFrameBuffer();
}

void FrameBuffer::Blit() const
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, _renderRelatedIds[MULTISAMPLING_FBO]);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _renderRelatedIds[NORMAL_FBO]);
	glBlitFramebuffer(0, 0, _size.x, _size.y, 0, 0, _size.x, _size.y, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

void FrameBuffer::Rescale(int w, int h)
{
	_size = Vec2i(w, h);

	/* Rescale multisampling texture */
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, _renderRelatedIds[MULTISAMPLING_TEXTURE]);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB, w, h, GL_TRUE);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

	/* Rescale multisampling rbo */
	glBindRenderbuffer(GL_RENDERBUFFER, _renderRelatedIds[MULTISAMPLING_RBO]);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, w, h);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	/* Rescale normal texture */
	glBindTexture(GL_TEXTURE_2D, _renderRelatedIds[NORMAL_TEXTURE]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void FrameBuffer::Destroy()
{
	glDeleteFramebuffers(1, &_renderRelatedIds[NORMAL_FBO]);
	glDeleteTextures(1, &_renderRelatedIds[NORMAL_TEXTURE]);
	glDeleteFramebuffers(1, &_renderRelatedIds[MULTISAMPLING_FBO]);
	glDeleteTextures(1, &_renderRelatedIds[MULTISAMPLING_TEXTURE]);
	glDeleteRenderbuffers(1, &_renderRelatedIds[MULTISAMPLING_RBO]);
	_frameBufferVAO->Destroy();
}
#endif

/* -----------------------------------------------------
 *          PRIVATE METHODS
 * -----------------------------------------------------
*/

#if 0
void FrameBuffer::InitFrameBuffer()
{
	InitFrameBufferVAO();

	glGenFramebuffers(1, &_renderRelatedIds[NORMAL_FBO]);
	glGenTextures(1, &_renderRelatedIds[NORMAL_TEXTURE]);
	glGenFramebuffers(1, &_renderRelatedIds[MULTISAMPLING_FBO]);
	glGenTextures(1, &_renderRelatedIds[MULTISAMPLING_TEXTURE]);
	glGenRenderbuffers(1, &_renderRelatedIds[MULTISAMPLING_RBO]);

	glBindFramebuffer(GL_FRAMEBUFFER, _renderRelatedIds[MULTISAMPLING_FBO]);

	/* Initialize multisampling texture */
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, _renderRelatedIds[MULTISAMPLING_TEXTURE]);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB16F, _size.x, _size.y, GL_TRUE);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, _renderRelatedIds[MULTISAMPLING_TEXTURE], 0);

	/* Initialize multisampling render buffer object  */
	glBindRenderbuffer(GL_RENDERBUFFER, _renderRelatedIds[MULTISAMPLING_RBO]);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, _size.x, _size.y);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _renderRelatedIds[MULTISAMPLING_RBO]);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		CONSOLE_ERROR("ERROR::FRAMEBUFFER MSAA Framebuffer is not complete!");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, _renderRelatedIds[NORMAL_FBO]);

	/* Initialize normal texture */
	glBindTexture(GL_TEXTURE_2D, _renderRelatedIds[NORMAL_TEXTURE]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, _size.x, _size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _renderRelatedIds[NORMAL_TEXTURE], 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		CONSOLE_ERROR("ERROR::FRAMEBUFFER Normal framebuffer is not complete!");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


#endif
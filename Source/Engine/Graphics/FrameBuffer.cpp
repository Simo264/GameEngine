#include "FrameBuffer.hpp"
#include "Engine/Graphics/Core/GL_Core.hpp"
#include "Engine/Graphics/Shader.hpp"
#include "Engine/Graphics/Renderer.hpp"
#include "Engine/Graphics/VertexArray.hpp"
#include "Core/Log/Logger.hpp"

/* -----------------------------------------------------
 *          PUBLIC METHODS
 * -----------------------------------------------------
*/

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

void FrameBuffer::BindMSAAFramebuffer() const 
{ 
	glBindFramebuffer(GL_FRAMEBUFFER, _renderRelatedIds[MULTISAMPLING_FBO]); 
}

void FrameBuffer::UnbindFrameBuffer() const 
{ 
	glBindFramebuffer(GL_FRAMEBUFFER, 0); 
};

void FrameBuffer::BlitFrameBuffer() const
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, _renderRelatedIds[MULTISAMPLING_FBO]);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _renderRelatedIds[NORMAL_FBO]);
	glBlitFramebuffer(0, 0, _size.x, _size.y, 0, 0, _size.x, _size.y, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

void FrameBuffer::DrawFrame(Shader* shader)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _renderRelatedIds[NORMAL_TEXTURE]);

	shader->SetInt("UPostProcessingType", (int)_postprocType);
	_frameBufferVAO->Bind();
	glDrawArrays(GL_TRIANGLES, 0, 6);
	_frameBufferVAO->Unbind();
	glBindTexture(GL_TEXTURE_2D, 0);
}
	
void FrameBuffer::RescaleFrameBuffer(int w, int h)
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

void FrameBuffer::DestroyFrameBuffer()
{
	glDeleteFramebuffers(1, &_renderRelatedIds[NORMAL_FBO]);
	glDeleteTextures(1, &_renderRelatedIds[NORMAL_TEXTURE]);
	glDeleteFramebuffers(1, &_renderRelatedIds[MULTISAMPLING_FBO]);
	glDeleteTextures(1, &_renderRelatedIds[MULTISAMPLING_TEXTURE]);
	glDeleteRenderbuffers(1, &_renderRelatedIds[MULTISAMPLING_RBO]);
	_frameBufferVAO->Destroy();
	delete _frameBufferVAO;
}


/* -----------------------------------------------------
 *          PRIVATE METHODS
 * -----------------------------------------------------
*/

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

void FrameBuffer::InitFrameBufferVAO()
{
	float vertices[] = {
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
	};

	VertexBufferConfig config; /* (2)position, (2)textCoords */
	config.PushAttributes({ 2,2 });
	VertexArrayData data{ sizeof(vertices),vertices,0,nullptr };
	_frameBufferVAO = new VertexArray(data, config);
}

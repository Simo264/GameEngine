#include "FrameBuffer.hh"
#include "Shader.hh"
#include "Renderer.hh"

#include "../Logger.hh"

/* -----------------------------------------------------
 *          PUBLIC METHODS
 * -----------------------------------------------------
*/

FrameBuffer::FrameBuffer(Vec2i size)
{
	glGenFramebuffers(1, &_renderRelatedIds[NORMAL_FBO]);
	glGenTextures(1, &_renderRelatedIds[NORMAL_TEXTURE]);
	glGenFramebuffers(1, &_renderRelatedIds[MULTISAMPLING_FBO]);
	glGenTextures(1, &_renderRelatedIds[MULTISAMPLING_TEXTURE]);
	glGenRenderbuffers(1, &_renderRelatedIds[MULTISAMPLING_RBO]);

	_postprocType = PostProcessingType::POST_PROC_NONE;

	float vertices[] = {
		// positions   // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
	};

	VertexArrayConfig config{ 2,2 }; // (2)position, (2)textCoords
	VertexArrayData data{ sizeof(vertices),vertices,0,nullptr };
	_screenFrameVAO.InitVertexArray(data, config);

	_size = size;
	InitializeFrameBuffer(size);
}

void FrameBuffer::BlitFrameBuffer()
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, _renderRelatedIds[MULTISAMPLING_FBO]);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _renderRelatedIds[NORMAL_FBO]);
	glBlitFramebuffer(0, 0, _size.x, _size.y, 0, 0, _size.x, _size.y, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

void FrameBuffer::DrawFrame(Shader* shader)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _renderRelatedIds[NORMAL_TEXTURE]);

	shader->SetInt("postProcessingType", (int)_postprocType);
	Renderer::DrawArrays(&_screenFrameVAO);

	glBindTexture(GL_TEXTURE_2D, 0);
}
	
void FrameBuffer::RescaleFrameBuffer(Vec2i newSize)
{
	_size = newSize;

	/* Rescale multisampling texture */
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, _renderRelatedIds[MULTISAMPLING_TEXTURE]);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB, newSize.x, newSize.y, GL_TRUE);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

	/* Rescale multisampling rbo */
	glBindRenderbuffer(GL_RENDERBUFFER, _renderRelatedIds[MULTISAMPLING_RBO]);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, newSize.x, newSize.y);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	/* Rescale normal texture */
	glBindTexture(GL_TEXTURE_2D, _renderRelatedIds[NORMAL_TEXTURE]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, newSize.x, newSize.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void FrameBuffer::Destroy()
{
	glDeleteFramebuffers(1, &_renderRelatedIds[MULTISAMPLING_FBO]);
	glDeleteFramebuffers(1, &_renderRelatedIds[NORMAL_FBO]);
	glDeleteTextures(1, &_renderRelatedIds[MULTISAMPLING_TEXTURE]);
	glDeleteTextures(1, &_renderRelatedIds[NORMAL_TEXTURE]);
	glDeleteRenderbuffers(1, &_renderRelatedIds[MULTISAMPLING_RBO]);
	_screenFrameVAO.DestroyVertexArray();
}


/* -----------------------------------------------------
 *          PUBLIC METHODS
 * -----------------------------------------------------
*/

void FrameBuffer::InitializeFrameBuffer(Vec2i size)
{
	glBindFramebuffer(GL_FRAMEBUFFER, _renderRelatedIds[MULTISAMPLING_FBO]);

	/* Initialize multisampling texture */
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, _renderRelatedIds[MULTISAMPLING_TEXTURE]);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB, size.x, size.y, GL_TRUE);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, _renderRelatedIds[MULTISAMPLING_TEXTURE], 0);
	
	/* Initialize multisampling render buffer object  */
	glBindRenderbuffer(GL_RENDERBUFFER, _renderRelatedIds[MULTISAMPLING_RBO]);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, size.x, size.y);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _renderRelatedIds[MULTISAMPLING_RBO]);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		CONSOLE_ERROR("ERROR::FRAMEBUFFER MSAA Framebuffer is not complete!");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, _renderRelatedIds[NORMAL_FBO]);

	/* Initialize normal texture */
	glBindTexture(GL_TEXTURE_2D, _renderRelatedIds[NORMAL_TEXTURE]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, size.x, size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _renderRelatedIds[NORMAL_TEXTURE], 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		CONSOLE_ERROR("ERROR::FRAMEBUFFER Normal framebuffer is not complete!");
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


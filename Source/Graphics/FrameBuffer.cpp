#include "FrameBuffer.hh"
#include "Shader.hh"
#include "Renderer.hh"

#include "../Logger.hh"

FrameBuffer::FrameBuffer(Vec2i size)
{
	_size = size;

	_renderRelatedIds[NORMAL_FBO] = 0;
	_renderRelatedIds[NORMAL_TEXTURE] = 0;
	_renderRelatedIds[NORMAL_RBO] = 0;
	_renderRelatedIds[MULTISAMPLING_FBO] = 0;
	_renderRelatedIds[MULTISAMPLING_TEXTURE] = 0;
	_renderRelatedIds[MULTISAMPLING_RBO] = 0;

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

	CreateFrameBuffer();
}

void FrameBuffer::CreateFrameBuffer()
{
	glGenFramebuffers(1, &_renderRelatedIds[NORMAL_FBO]);
	glBindFramebuffer(GL_FRAMEBUFFER, _renderRelatedIds[NORMAL_FBO]);
	
	/* Create a multisampling texture */
	glGenTextures(1, &_renderRelatedIds[NORMAL_TEXTURE]);
	glBindTexture(GL_TEXTURE_2D, _renderRelatedIds[NORMAL_TEXTURE]);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, _size.x, _size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	
	/* Create a color renderbuffer object for both depth and stencil attachment */
	glGenRenderbuffers(1, &_renderRelatedIds[NORMAL_RBO]);
	glBindRenderbuffer(GL_RENDERBUFFER, _renderRelatedIds[NORMAL_RBO]);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, _size.x, _size.y);

	/* Attach the texture and renderbuffer to the FBO */
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _renderRelatedIds[NORMAL_TEXTURE], 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _renderRelatedIds[NORMAL_RBO]);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		CONSOLE_ERROR("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::BlitFrameBuffer()
{

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
	glBindTexture(GL_TEXTURE_2D, _renderRelatedIds[NORMAL_TEXTURE]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, newSize.x, newSize.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _renderRelatedIds[NORMAL_TEXTURE], 0);

	glBindRenderbuffer(GL_RENDERBUFFER, _renderRelatedIds[NORMAL_TEXTURE]);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, newSize.x, newSize.y);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _renderRelatedIds[NORMAL_TEXTURE]);
}

void FrameBuffer::Destroy()
{

	_screenFrameVAO.DestroyVertexArray();
}

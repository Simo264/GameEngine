#pragma once

#include "Core.hh"
#include "UncopyableObject.hh"
#include "VertexArray.hh"

enum class PostProcessingType {
	POST_PROC_NONE			= 0,
	POST_PROC_INVERSION = 1,
	POST_PROC_GRAYSCALE = 2,
	POST_PROC_KERNEL		= 3,
	POST_PROC_BLUR			= 4,
};


class FrameBuffer : public UncopyableObject
{
public:
	FrameBuffer();
	void InitializeFrameBuffer(Vec2i size);
	
	void BindMSAAFramebuffer() const { glBindFramebuffer(GL_FRAMEBUFFER, _renderRelatedIds[MULTISAMPLING_FBO]);  }
	void UnbindFrameBuffer() const { glBindFramebuffer(GL_FRAMEBUFFER, 0); };
	void BlitFrameBuffer();
	
	uint32_t GetFramebufferTexture() const { return _renderRelatedIds[NORMAL_TEXTURE]; }

	void DrawFrame(class Shader* shader);
	void SetPostProcessing(PostProcessingType type) { _postprocType = type; }
	void RescaleFrameBuffer(Vec2i newSize);
	void DestroyFrameBuffer();

private:
	Vec2i _size; 

	enum FBORenderTarget : int
	{
		NORMAL_FBO = 0,
		NORMAL_TEXTURE,

		MULTISAMPLING_FBO,
		MULTISAMPLING_TEXTURE,
		MULTISAMPLING_RBO,
	};
	uint32_t _renderRelatedIds[5];
	VertexArray _frameBufferVAO;
	PostProcessingType _postprocType;

	void InitFrameBufferVAO();
};

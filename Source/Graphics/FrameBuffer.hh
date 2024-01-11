#pragma once

#include "../Core.hh"
#include "../UncopyableObject.hh"
#include "../Graphics/VertexArray.hh"

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
	FrameBuffer(Vec2i size);
	
	void BindFrameBuffer() const { glBindFramebuffer(GL_FRAMEBUFFER, _renderRelatedIds[NORMAL_FBO]); };
	void UnbindFrameBuffer() const { glBindFramebuffer(GL_FRAMEBUFFER, 0); };
	void DrawFrame(class Shader* shader);
	void BlitFrameBuffer();

	void SetPostProcessing(PostProcessingType type) { _postprocType = type; }

	uint32_t GetFramebufferTexture() const { return _renderRelatedIds[NORMAL_TEXTURE]; }
	void RescaleFrameBuffer(Vec2i newSize);

	void Destroy();

private:
	Vec2i _size;
	
	enum FBORenderTarget : int
	{
		NORMAL_FBO = 0,
		NORMAL_TEXTURE,
		NORMAL_RBO,

		MULTISAMPLING_FBO,
		MULTISAMPLING_TEXTURE,
		MULTISAMPLING_RBO,
	};
	uint32_t _renderRelatedIds[8];
	VertexArray _screenFrameVAO;
	PostProcessingType _postprocType;

	void CreateFrameBuffer();
};

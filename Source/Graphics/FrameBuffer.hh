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
	FrameBuffer();

	void CreateFrameBuffer(Vec2i size);
	void BindFrameBuffer() const { glBindFramebuffer(GL_FRAMEBUFFER, _fbo); };
	void UnbindFrameBuffer() const { glBindFramebuffer(GL_FRAMEBUFFER, 0); };
	void DrawFrame(class Shader* shader);
	void SetPostProcessing(PostProcessingType type) { _postprocType = type; }

	const uint32_t& GetFramebufferTexture() const { return _texture; }
	void RescaleFrameBuffer(Vec2i newSize);

	void Destroy();
	
private:
	uint32_t _fbo;
	uint32_t _rbo;
	uint32_t _texture;
	VertexArray _screenFrameVAO;
	PostProcessingType _postprocType;
};

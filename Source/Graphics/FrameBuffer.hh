#pragma once

#include "../Core.hh"
#include "Shader.hh"
#include "../Mesh/Mesh.hh"

enum class PostProcessingType {
	POST_PROC_NONE			= 0,
	POST_PROC_INVERSION = 1,
	POST_PROC_GRAYSCALE = 2,
	POST_PROC_KERNEL		= 3,
	POST_PROC_BLUR			= 4,
};


class FrameBuffer
{
public:
	FrameBuffer();
	~FrameBuffer() = default;

	FrameBuffer(const FrameBuffer&) = delete;            // delete copy constructor
	FrameBuffer& operator=(const FrameBuffer&) = delete; // delete assign op

	void Create(vec2i size);
	void Bind()		const { glBindFramebuffer(GL_FRAMEBUFFER, _fbo); };
	void Unbind() const { glBindFramebuffer(GL_FRAMEBUFFER, 0); };
	void DrawFrame(Shader* shader);
	void SetPostProcessing(PostProcessingType type) { _postprocType = type; }

	const uint32_t& GetFramebufferTexture() const { return _texture; }
	void RescaleFrameBuffer(vec2i newSize);

	void Destroy();
	
private:
	uint32_t _fbo;
	uint32_t _rbo;
	uint32_t _texture;

	Mesh _screenFrame;

	PostProcessingType _postprocType;
};

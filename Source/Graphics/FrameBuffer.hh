#pragma once

#include "../Core.hh"

class FrameBuffer
{
public:
	FrameBuffer(vec2i size);
	~FrameBuffer() = default;

	FrameBuffer(const FrameBuffer&) = delete;            // delete copy constructor
	FrameBuffer& operator=(const FrameBuffer&) = delete; // delete assign op

	void Bind()		const { glBindFramebuffer(GL_FRAMEBUFFER, _fbo); };
	void Unbind() const { glBindFramebuffer(GL_FRAMEBUFFER, 0); };

	const uint32_t& GetFramebufferTexture() const { return _texture; }
	void RescaleFrameBuffer(vec2i newSize);

	void Destroy();
	
private:
	uint32_t _fbo;
	uint32_t _rbo;
	uint32_t _texture;
};

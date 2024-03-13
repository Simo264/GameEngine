#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Math.hpp"

enum class PostProcessingType : int {
	POST_PROC_NONE			= 0,
	POST_PROC_INVERSION = 1,
	POST_PROC_GRAYSCALE = 2,
	POST_PROC_KERNEL		= 3,
	POST_PROC_BLUR			= 4,
};

/* -----------------------------------------------------------------------------------------
	So far we've used several types of screen buffers: 
		1. color buffer for writing color values;
		2. depth buffer to write and test depth information
		3. stencil buffer that allows us to discard certain fragments based on some condition. 

	The combination of these buffers is stored in GPU memory and is called a framebuffer. 

	Now each buffer attached to a Framebuffer can be a Renderbuffer or a texture.
		1. Texture:
			when attaching a texture to a framebuffer, all rendering commands will write to the texture.
			The advantage of using textures is that the render output is stored inside
			the texture image that we can then easily use in our shaders.
		2. Renderbuffer:
			renderbuffer objects were introduced to OpenGL after textures as a possible type of 
			framebuffer attachment, Just like a texture image, a renderbuffer object is an actual buffer. 
			Stores pixel values in native format, so it's optimized for offscreen rendering. 
			In other words, drawing to a Renderbuffer can be much faster than drawing to a texture.
			The drawback is that pixels uses a native so that reading from a Renderbuffer is much harder 
			than reading from a texture.

			However, a renderbuffer object can not be directly read from. 
			This gives it the added advantage that OpenGL can do a few memory optimizations that can give it 
			a performance edge over textures for off-screen rendering to a framebuffer.

			Nevertheless, once a Renderbuffer has been painted, one can copy its content directly to screen
			very quickly using pixel transfer operations.
			This means that a Renderbuffer can be used to efficiently implement the double buffer pattern.
	----------------------------------------------------------------------------------------- */
class FrameBuffer
{
public:
	FrameBuffer(const Vec2i& size);
	~FrameBuffer();

	/* By binding to the GL_FRAMEBUFFER target all the next read 
		 and write framebuffer operations will affect the currently bound framebuffer. 
		 It is also possible to bind a framebuffer to a read or write target specifically 
		 by calling to BindRead() or BindWrite(). */
	void Bind() const;
	void Unbind() const;
	
	/* The framebuffer bound to GL_READ_FRAMEBUFFER is then used for all read operations 
		 like glReadPixels */
	void BindRead() const;
	void UnbindRead() const;

	/* The framebuffer bound to GL_DRAW_FRAMEBUFFER is used as the destination for rendering, 
		 clearing and other write operations */
	void BindWrite() const;
	void UnbindWrite() const;
	
	void AttachColorBuffer();
	void AttachDepthBuffer();
	void AttachStencilBuffer();

	bool CheckStatus() const;

	//void Blit() const;
	
	constexpr uint32_t GetImage() const { return _textureColorBuffer; }
	constexpr Vec2i GetSize() const { return _size; }
	constexpr float GetAspect() const { return static_cast<float>(_size.x) / static_cast<float>(_size.y); }

	//void SetPostProcessing(PostProcessingType type) { _postprocType = type; }
	
	/* Resize the frame buffer viewport */
	void Rescale(Vec2i size);

private:
	Vec2i _size;

	uint32_t _fbo; /* frame buffer id */

	/* Attachments */
	uint32_t _textureColorBuffer;	
	uint32_t _textureDepthBuffer;
	uint32_t _textureStencilBuffer;

	

#if 0
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
	
	class VertexArray* _frameBufferVAO;
	PostProcessingType _postprocType;

	void InitFrameBuffer();
	void InitFrameBufferVAO();
#endif
};

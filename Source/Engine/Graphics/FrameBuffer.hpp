#pragma once

#include "Core.hpp"
#include "Engine/Graphics/VertexArray.hpp"

enum class PostProcessingType {
	POST_PROC_NONE			= 0,
	POST_PROC_INVERSION = 1,
	POST_PROC_GRAYSCALE = 2,
	POST_PROC_KERNEL		= 3,
	POST_PROC_BLUR			= 4,
};

/* -------------------------------------------
			Frame buffer class 
	------------------------------------------- */
class FrameBuffer
{
/* How to use Framebuffer:
	1. Create frame buffer object:
		FrameBuffer framebuffer;
		framebuffer.InitFrameBuffer(buffersize);

	2. Use frame buffer object:
		framebuffer.BindMSAAFramebuffer();
		glClearColor(pow(0.1f, GAMMA_CORRECTION), pow(0.1f, GAMMA_CORRECTION), pow(0.1f, GAMMA_CORRECTION), 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

	3. Render scene to frame buffer object
		scene.draw()

	4. Unbind frame buffer object
		framebuffer.BlitFrameBuffer();
		framebuffer.UnbindFrameBuffer();

	5.1 Draw frame buffer image as texture
		ImGui::Image((ImTextureID)framebuffer.GetImage(),
			wsize,
			ImVec2(0, 1),
			ImVec2(1, 0));

	5.2 Or draw on screen
		framebufferShader->Use();
		framebuffer.DrawFrame(framebufferShader);
*/

public:
	FrameBuffer();
	void InitFrameBuffer(Vec2i size);
	
	void BindMSAAFramebuffer() const { glBindFramebuffer(GL_FRAMEBUFFER, _renderRelatedIds[MULTISAMPLING_FBO]);  }
	void UnbindFrameBuffer() const { glBindFramebuffer(GL_FRAMEBUFFER, 0); };
	void BlitFrameBuffer();
	
	uint32_t GetImage() const { return _renderRelatedIds[NORMAL_TEXTURE]; }
	Vec2i GetSize() const { return _size; }
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

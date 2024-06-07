#pragma once

#include "Core/Core.hpp"

#include "Engine/Graphics/Buffer.hpp"
#include "Engine/Graphics/VertexArray.hpp"
#include "Engine/Graphics/FrameBuffer.hpp"

class Engine
{
public:
	Engine() = default;
	~Engine() = default;

	void Initialize();
	void Run();
	void CleanUp();

private:
	void SetOpenGLStates();

	void LoadTextures() const;
	void CreateFramebuffer(int samples, int width, int height);
	void CreateFramebufferShadowMap(int width, int height);
	void CreateScreenSquare();
	void ResizeFramebuffer(vec2i32 newViewportSize);

	class WindowManager*	_instanceWM;
	class ShaderManager*	_instanceSM;
	class TextureManager* _instanceTM;

	FrameBuffer _fboMultisampled;
	FrameBuffer _fboIntermediate;
	FrameBuffer _fboShadowMap;

	Buffer _uboCamera; /* uniform cameraBlock object */

	VertexArray _screenSquare;
	vec2i32 _viewport;
};

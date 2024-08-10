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
	void CreateFramebuffer(int samples, int width, int height);
	void CreateScreenSquare();

	FrameBuffer _fboMultisampled;
	FrameBuffer _fboIntermediate;

	Buffer _uboCamera; /* Uniform cameraBlock object */

	VertexArray _screenSquare;
	vec2i32 _viewportSize;
};

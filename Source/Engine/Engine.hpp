#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Base.hpp"

#include "Engine/Graphics/Objects/Buffer.hpp"
#include "Engine/Graphics/Containers/VertexArray.hpp"
#include "Engine/Graphics/Containers/FrameBuffer.hpp"

class Engine
{
public:
	Engine() = default;
	~Engine() = default;

	void Initialize();
	void Run();
	void CleanUp();

private:
	void CreateFramebuffer(i32 samples, i32 width, i32 height);
	void CreateScreenSquare();

	FrameBuffer _fboMultisampled;
	FrameBuffer _fboIntermediate;

	Buffer _uboCameraBlock;	// UBO "CameraBlock"
	Buffer _uboLightBlock;	// UBO "LightBlock"
	Buffer _uboBoneBlock;		// UBO "BoneBlock"

	VertexArray _screenSquare;
	vec2i _viewportSize;
};

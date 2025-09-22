#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Base.hpp"

#include "Engine/Components/StaticMesh.hpp"
#include "Engine/Graphics/FrameBuffer.hpp"

class Engine
{
public:
	Engine() = default;
	~Engine() = default;

	void Initialize();
	void Run();
	void Cleanup();

private:
	void __SetInitialGLStates() const;
	void __InitTime();
	void __CalculatePerFrameTime();
	void __CreateCameraUBO(i32 bindingPoint);
	void __CreateLightUBO(i32 bindingPoint);
	Components::StaticMesh __CreateMeshQuad();
	void __CreateMSAAFramebuffer(i32 w, i32 h, i32 samples);
	void __ResolveMSAAFramebuffer(i32 w, i32 h);

	Framebuffer _MSAAFramebuffer;
	Framebuffer _MSAAFramebufferResolver;
	i32 _MSAAFramebufferSamples;
	Components::StaticMesh _screenQuad;
	Buffer _uboCameraBlock;	// UBO "CameraBlock"	-> bindpoint 0
	Buffer _uboLightBlock;	// UBO "LightBlock"		-> bindpoint 1
	
	chrono::steady_clock::time_point _now;
	chrono::steady_clock::time_point _lastFrameTime;
	chrono::steady_clock::time_point _timerT0;
	chrono::steady_clock::time_point _timerT1;
	u32 _frames;
	u32 _frameRate; // How many frames generated per seconds
	f64 _totalDeltasPerSecond;
	f64 _avgTime; // The average rendering time per seconds
	f64 _delta; // Time elapsed between two frames
};

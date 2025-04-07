#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Base.hpp"

#include "Engine/Graphics/Containers/FrameBuffer.hpp"
#include "Engine/Graphics/Mesh.hpp"

class Engine
{
public:
	Engine() = default;
	~Engine() = default;

	void Initialize();
	void Run();
	void CleanUp();

private:
	void SetGLStates() const;
	void CreateCameraUBO();
	void CreateLightUBO();
	void CreateBoneUBO();
	void InitTime();

	void CreateFramebuffer(i32 samples, i32 width, i32 height);
	void CreateScreenSquare();
	class TextureCubemap CreateSkybox(const Array<Texture2D, 6>& faces);

	void CalculatePerFrameTime();
	

	FrameBuffer _fboMultisampled;
	FrameBuffer _fboIntermediate;

	Buffer _uboCameraBlock;	// UBO "CameraBlock"	-> bindpoint 0
	Buffer _uboLightBlock;	// UBO "LightBlock"		-> bindpoint 1
	Buffer _uboBoneBlock;		// UBO "BoneBlock"		-> bindpoint 2

	Mesh _screenSquare;
	Mesh _meshCubeSkybox;
	
	vec2i _viewportSize;
	
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

#pragma once

#include "Core/Core.hpp"

#include "Engine/Graphics/VertexArray.hpp"
#include "Engine/Graphics/FrameBuffer.hpp"

#include "Engine/Subsystems/WindowManager.hpp"
#include "Engine/Subsystems/ShaderManager.hpp"
#include "Engine/Subsystems/TextureManager.hpp"

#include "GUI/Gui.hpp"

class Engine
{
public:
	Engine() = default;
	~Engine() = default;

	void Initialize();
	void Run();
	void CleanUp();

	WindowManager* instanceWM;
	ShaderManager* instanceSM;
	TextureManager* instanceTM;

	FrameBuffer fboMultisampled;
	FrameBuffer fboIntermediate;
	VertexArray screenSquare;
	vec2i32 viewport;

	class Texture2D* framebufferTexture;

	Gui gui;

private:
	void LoadShaders() const;
	void LoadPrograms() const;
	void LoadTextures() const;
	void CreateFramebuffer(int samples, int width, int height);
	void CreateScreenSquare();
};

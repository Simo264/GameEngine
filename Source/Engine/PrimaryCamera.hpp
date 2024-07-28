#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Math.hpp"
#include "Engine/Components.hpp"

class PrimaryCamera
{
public:
	PrimaryCamera(
		const vec3f& position = { 0.0f, 0.0f, 0.0f },
		float fov = 45.0f,	
		float aspect = 16.0f / 9.0f,
		float znear = 1.0f,
		float zfar = 10.0f
	);
	~PrimaryCamera();

	void ProcessInput(float delta);

	Components::Camera* cameraComponent;

private:
	void Move(const float movementSpeed, float delta);
	void Rotate(const float mouseSensitivity, float delta);
};
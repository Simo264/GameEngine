#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Math.hpp"

class Camera
{
public:
	Camera(
		const vec3f& position = { 0.0f, 0.0f, 0.0f },
		float fov = 45.0f,	
		float aspect = 16.0f / 9.0f,
		float znear = 1.0f,
		float zfar = 10.0f
	);
	~Camera();

	void ProcessInput(float delta);

	class CameraComponent* cameraComponent;

private:
	void Move(class WindowManager& window, const float movementSpeed, float delta);
	void Rotate(class WindowManager& window, const float mouseSensitivity, float delta);
};
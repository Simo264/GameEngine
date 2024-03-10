#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Math.hpp"

class Camera
{
public:
	Camera();
	Camera(const Vec3f& position, float fov, float aspect);

	~Camera();

	void ProcessInput(class WindowManager& window, float delta);

	class CameraComponent* cameraComponent;

private:
	void Move(class WindowManager& window, const float movementSpeed, float delta);
	void Rotate(class WindowManager& window, const float mouseSensitivity, float delta);
};
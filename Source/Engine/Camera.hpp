#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Math.hpp"

class Camera
{
public:
	Camera(
		const Vec3f& position = { 0.0f, 0.0f, 0.0f }, /* default position*/
		float fov = 45.0f,						/* default field of view */
		float aspect = 16.0f / 9.0f		/* default aspect ratio 16:9 */
	);

	~Camera();

	void ProcessInput(class WindowManager& window, float delta);

	class CameraComponent* cameraComponent;

private:
	void Move(class WindowManager& window, const float movementSpeed, float delta);
	void Rotate(class WindowManager& window, const float mouseSensitivity, float delta);
};
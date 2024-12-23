#include "Engine/Camera.hpp"

#include "Core/Math/Extensions.hpp"

#include "Engine/Globals.hpp"
#include "Engine/Subsystems/WindowManager.hpp"

#include <GLFW/glfw3.h>

/* -----------------------------------------------------
 *          PUBLIC METHODS
 * -----------------------------------------------------
*/

Camera::Camera(vec3f position, f32 yaw, f32 pitch, f32 roll)
	: position{ position },
		yaw{ yaw },
		pitch{ pitch },
		roll{ roll },
		fov{ 45.0f }
{
	UpdateOrientation();
}

void Camera::UpdateOrientation()
{
	vec3f direction{};
	direction.x = Math::Cos(Math::Radians(yaw)) * Math::Cos(Math::Radians(pitch));
	direction.y = Math::Sin(Math::Radians(pitch));
	direction.z = Math::Sin(Math::Radians(yaw)) * Math::Cos(Math::Radians(pitch));

	_front = Math::Normalize(direction);

	_right = Math::Normalize(Math::Cross(_front, vec3f(0.0f, 1.0f, 0.0f)));

	const mat4f rollMat = Math::Rotate(mat4f(1.0f), Math::Radians(roll), _front);
	_up = Math::Normalize(Math::Cross(_right, _front));
	_up = mat3f(rollMat) * _up;
}

mat4f Camera::CalculateView(vec3f center) const
{
	return Math::LookAt(position, center, _up);
}

mat4f Camera::CalculatePerspective(f32 aspect) const
{
	return Math::Perspective(Math::Radians(fov), aspect, frustum.zNear, frustum.zFar);
}

mat4f Camera::CalculateOrtho() const
{
	return Math::Ortho(frustum.left, frustum.right, frustum.bottom, frustum.top, frustum.zNear, frustum.zFar);
}

void Camera::ProcessKeyboard(f32 delta, f32 movementSpeed)
{
	WindowManager& windowManager = WindowManager::Get();

	const f32 velocity = movementSpeed * delta;

	/* W-S */
	if (windowManager.GetKey(GLFW_KEY_W) == GLFW_PRESS)
	{
		position += _front * velocity;
	}
	else if (windowManager.GetKey(GLFW_KEY_S) == GLFW_PRESS)
	{
		position -= _front * velocity;
	}

	/* A-D */
	if (windowManager.GetKey(GLFW_KEY_A) == GLFW_PRESS)
	{
		position -= _right * velocity;
	}
	else if (windowManager.GetKey(GLFW_KEY_D) == GLFW_PRESS)
	{
		position += _right * velocity;
	}

	/* SPACE-LCTRL */
	if (windowManager.GetKey(GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		position += _up * velocity;
	}
	else if (windowManager.GetKey(GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		position -= _up * velocity;
	}
}

void Camera::ProcessMouse(f32 delta, f32 mouseSensitivity)
{
	WindowManager& windowManager = WindowManager::Get();

	if (windowManager.GetMouseKey(GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{
		vec2d mousePos = windowManager.GetCursorPosition();
		vec2i32 windowSize = windowManager.GetWindowSize();
		static f32 lastX = (f32)windowSize.x / 2.0f;
		static f32 lastY = (f32)windowSize.y / 2.0f;
		static bool firstMouse = true;
		if (firstMouse)
		{
			lastX = mousePos.x;
			lastY = mousePos.y;
			firstMouse = false;
		}

		const f32 velocity = mouseSensitivity * delta * 10;

		const f32 xoffset = lastX - mousePos.x;
		if (xoffset < 0) /* Right */
			yaw += velocity;
		else if (xoffset > 0) /* Left */
			yaw -= velocity;

		const f32 yoffset = lastY - mousePos.y;
		if (yoffset > 0) /* Up */
			pitch += velocity;
		else if (yoffset < 0) /* Down */
			pitch -= velocity;

		lastX = mousePos.x;
		lastY = mousePos.y;

		/* make sure that when pitch is out of bounds, screen doesn't get flipped */
		static bool constrainPitch = true;
		if (constrainPitch)
		{
			if (pitch > 89.0f)
				pitch = 89.0f;
			if (pitch < -89.0f)
				pitch = -89.0f;
		}
	}
}

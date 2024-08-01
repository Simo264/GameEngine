#include "Engine/Camera.hpp"
#include "Core/Math/Extensions.hpp"
#include "Engine/Globals.hpp"
#include "Engine/Subsystems/WindowManager.hpp"

#include <GLFW/glfw3.h>

/* -----------------------------------------------------
 *          PUBLIC METHODS
 * -----------------------------------------------------
*/

Camera::Camera(vec3f position, float yaw, float pitch, float roll)
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

mat4f Camera::CalculatePerspective(float aspect) const
{
	return Math::Perspective(fov, aspect, frustum.zNear, frustum.zFar);
}

mat4f Camera::CalculateOrtho() const
{
	return Math::Ortho(frustum.left, frustum.right, frustum.bottom, frustum.top, frustum.zNear, frustum.zFar);
}

void Camera::ProcessKeyboard(float delta)
{
	constexpr static float movementSpeed = 7.5f;
	const float velocity = movementSpeed * delta;

	/* W-S */
	if (g_windowManager.GetKey(GLFW_KEY_W) == GLFW_PRESS)
	{
		position += _front * velocity;
	}
	else if (g_windowManager.GetKey(GLFW_KEY_S) == GLFW_PRESS)
	{
		position -= _front * velocity;
	}

	/* A-D */
	if (g_windowManager.GetKey(GLFW_KEY_A) == GLFW_PRESS)
	{
		position -= _right * velocity;
	}
	else if (g_windowManager.GetKey(GLFW_KEY_D) == GLFW_PRESS)
	{
		position += _right * velocity;
	}

	/* SPACE-LCTRL */
	if (g_windowManager.GetKey(GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		position += _up * velocity;
	}
	else if (g_windowManager.GetKey(GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		position -= _up * velocity;
	}
}

void Camera::ProcessMouse(float delta)
{
	constexpr static float mouseSensitivity = 50.0f;

	if (g_windowManager.GetMouseKey(GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{
		vec2d mousePos = g_windowManager.GetCursorPosition();
		vec2i32 windowSize = g_windowManager.GetWindowSize();
		static float lastX = (float)windowSize.x / 2.0f;
		static float lastY = (float)windowSize.y / 2.0f;
		static bool firstMouse = true;
		if (firstMouse)
		{
			lastX = mousePos.x;
			lastY = mousePos.y;
			firstMouse = false;
		}

		const float velocity = mouseSensitivity * delta * 10;

		const float xoffset = lastX - mousePos.x;
		if (xoffset < 0) /* Right */
			yaw += velocity;
		else if (xoffset > 0) /* Left */
			yaw -= velocity;

		const float yoffset = lastY - mousePos.y;
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

#include "Engine/Camera.hpp"

#include "Core/Math/Ext.hpp"

#include "Engine/Globals.hpp"
#include "Engine/Subsystems/WindowManager.hpp"

#include <GLFW/glfw3.h>

/* -----------------------------------------------------
 *          PUBLIC METHODS
 * -----------------------------------------------------
*/

Camera::Camera(vec3f position, vec3f orientation, f32 fov)
	: position{ position },
		orientation{ orientation },
		fov{ fov }
{
	UpdateOrientation();
}

void Camera::UpdateOrientation()
{
	f32 yaw = orientation.x;
	f32 pitch = orientation.y;
	f32 roll = orientation.z;

	vec3f direction{};
	direction.x = glm::cos(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
	direction.y = glm::sin(glm::radians(pitch));
	direction.z = glm::sin(glm::radians(yaw)) * glm::cos(glm::radians(pitch));

	_front = glm::normalize(direction);
	_right = glm::normalize(glm::cross(_front, vec3f(0.0f, 1.0f, 0.0f)));

	const mat4f rollMat = glm::rotate(mat4f(1.0f), glm::radians(roll), _front);
	_up = glm::normalize(glm::cross(_right, _front));
	_up = mat3f(rollMat) * _up;
}

mat4f Camera::CalculateView(vec3f center) const
{
	return glm::lookAt(position, center, _up);
}

mat4f Camera::CalculatePerspective(f32 aspect) const
{
	return glm::perspective(glm::radians(fov), aspect, frustum.zNear, frustum.zFar);
}

mat4f Camera::CalculateOrtho() const
{
	return glm::ortho(frustum.left, frustum.right, frustum.bottom, frustum.top, frustum.zNear, frustum.zFar);
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
		vec2i windowSize = windowManager.GetWindowSize();
		static f32 lastX = (f32)windowSize.x / 2.0f;
		static f32 lastY = (f32)windowSize.y / 2.0f;
		static bool firstMouse = true;
		if (firstMouse)
		{
			lastX = mousePos.x;
			lastY = mousePos.y;
			firstMouse = false;
		}

		f32& yaw = orientation.x;
		f32& pitch = orientation.y;
		f32& roll = orientation.z;

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

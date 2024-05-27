#include "Camera.hpp"

#include "Engine/Components.hpp"

#include "Engine/Subsystems/WindowManager.hpp"

#include <GLFW/glfw3.h>

/* -----------------------------------------------------
 *          PUBLIC METHODS
 * -----------------------------------------------------
*/

Camera::Camera(const vec3f& position, float fov, float aspect) 
{
	cameraComponent = new CameraComponent(position, fov, aspect);
}

Camera::~Camera() 
{ 
	delete cameraComponent; 
}

void Camera::ProcessInput(WindowManager& window, float delta)
{
	const static float movementSpeed = 7.5f;
	const static float mouseSensitivity = 25.0f; 

	Move(window, movementSpeed, delta);
	if (window.GetMouseKey(GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{
		Rotate(window, mouseSensitivity, delta);
		cameraComponent->UpdateVectors();
	}
	cameraComponent->UpdateView();
}

/* -----------------------------------------------------
 *          PRIVATE METHODS
 * -----------------------------------------------------
*/

void Camera::Move(WindowManager& window, const float movementSpeed, float delta)
{
	const float velocity = movementSpeed * delta;

	/* W-S */
	if (window.GetKey(GLFW_KEY_W) == GLFW_PRESS)
	{
		cameraComponent->position += cameraComponent->GetFrontVector() * velocity;
	}
	else if (window.GetKey(GLFW_KEY_S) == GLFW_PRESS)
	{
		cameraComponent->position -= cameraComponent->GetFrontVector() * velocity;
	}

	/* A-D */
	if (window.GetKey(GLFW_KEY_A) == GLFW_PRESS)
	{
		cameraComponent->position -= cameraComponent->GetRightVector() * velocity;
	}
	else if (window.GetKey(GLFW_KEY_D) == GLFW_PRESS)
	{
		cameraComponent->position += cameraComponent->GetRightVector() * velocity;
	}

	/* SPACE-LCTRL */
	if (window.GetKey(GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		cameraComponent->position += cameraComponent->GetUpVector() * velocity;
	}
	else if (window.GetKey(GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		cameraComponent->position -= cameraComponent->GetUpVector() * velocity;
	}
}

void Camera::Rotate(WindowManager& window, const float mouseSensitivity, float delta)
{
	vec2d mousePos = window.GetCursorPosition();
	vec2i32 windowSize = window.GetWindowSize();
	static float lastX = ((float)windowSize.x / 2.0f);
	static float lastY = ((float)windowSize.y / 2.0f);
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
		cameraComponent->yaw += velocity;
	else if (xoffset > 0) /* Left */
		cameraComponent->yaw -= velocity;

	const float yoffset = lastY - mousePos.y;
	if (yoffset > 0) /* Up */
		cameraComponent->pitch += velocity;
	else if (yoffset < 0) /* Down */
		cameraComponent->pitch -= velocity;

	lastX = mousePos.x;
	lastY = mousePos.y;

	/* make sure that when pitch is out of bounds, screen doesn't get flipped */
	static bool constrainPitch = true;
	if (constrainPitch)
	{
		if (cameraComponent->pitch > 89.0f)
			cameraComponent->pitch = 89.0f;
		if (cameraComponent->pitch < -89.0f)
			cameraComponent->pitch = -89.0f;
	}
}

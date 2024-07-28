#include "PrimaryCamera.hpp"

#include "Engine/Globals.hpp"
#include "Engine/Subsystems/WindowManager.hpp"

#include <GLFW/glfw3.h>

constexpr static float movementSpeed = 7.5f;
constexpr static float mouseSensitivity = 50.0f;

/* -----------------------------------------------------
 *          PUBLIC METHODS
 * -----------------------------------------------------
*/

PrimaryCamera::PrimaryCamera(const vec3f& position, float fov, float aspect, float znear, float zfar)
{
	cameraComponent = new Components::Camera(position, fov, aspect, znear, zfar);
}

PrimaryCamera::~PrimaryCamera() 
{ 
	delete cameraComponent; 
}

void PrimaryCamera::ProcessInput(float delta)
{
	Move(movementSpeed, delta);
	if (g_windowManager.GetMouseKey(GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{
		Rotate(mouseSensitivity, delta);
		cameraComponent->UpdateVectors();
	}
	cameraComponent->UpdateView();
}

/* -----------------------------------------------------
 *          PRIVATE METHODS
 * -----------------------------------------------------
*/

void PrimaryCamera::Move(const float movementSpeed, float delta)
{
	const float velocity = movementSpeed * delta;

	/* W-S */
	if (g_windowManager.GetKey(GLFW_KEY_W) == GLFW_PRESS)
	{
		cameraComponent->position += cameraComponent->GetFrontVector() * velocity;
	}
	else if (g_windowManager.GetKey(GLFW_KEY_S) == GLFW_PRESS)
	{
		cameraComponent->position -= cameraComponent->GetFrontVector() * velocity;
	}

	/* A-D */
	if (g_windowManager.GetKey(GLFW_KEY_A) == GLFW_PRESS)
	{
		cameraComponent->position -= cameraComponent->GetRightVector() * velocity;
	}
	else if (g_windowManager.GetKey(GLFW_KEY_D) == GLFW_PRESS)
	{
		cameraComponent->position += cameraComponent->GetRightVector() * velocity;
	}

	/* SPACE-LCTRL */
	if (g_windowManager.GetKey(GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		cameraComponent->position += cameraComponent->GetUpVector() * velocity;
	}
	else if (g_windowManager.GetKey(GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		cameraComponent->position -= cameraComponent->GetUpVector() * velocity;
	}
}
void PrimaryCamera::Rotate(const float mouseSensitivity, float delta)
{
	vec2d mousePos = g_windowManager.GetCursorPosition();
	vec2i32 windowSize = g_windowManager.GetWindowSize();
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

#include "Camera.hpp"

#include "Core/Math/Extensions.hpp"
#include "Core/Log/Logger.hpp"

#include "Engine/Subsystems/WindowManager.hpp"
#include <GLFW/glfw3.h>

/* -----------------------------------------------------
 *          PUBLIC METHODS
 * -----------------------------------------------------
*/

Camera::Camera(Vec3f position, float fov)
  : position{ position },
    fov{ fov },
    yaw{ -90.0f },
    pitch{ 0.0f },
    movementSpeed{ 7.5f },        /* [1.0f:10.0f] */
    mouseSensitivity{ 25.0f },    /* [1.0f:100.0f] */
    _front{ 0.0f, 1.0f, 0.0f },
    _worldUp{ 0.0f, 1.0f, 0.0f }
{
  ;
  Vec2i windowSize; 
  WindowManager::Instance().GetWindowSize(windowSize.x, windowSize.y);
  
  _lastX = ((float)windowSize.x / 2.0f);
  _lastY = ((float)windowSize.y / 2.0f);

  UpdateCameraVectors();

  _viewMatrix = Math::LookAt(position, position + _front, _up);
}

void Camera::ProcessInput(const double deltaTime)
{
  auto& window = WindowManager::Instance();

  Vec2d mousePos;
  window.GetCursorPosition(mousePos.x, mousePos.y);

  FreeCameraWalk(&window, deltaTime);
  
  if (window.GetMouseKey(GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
  {
    FreeCameraRotation(mousePos, deltaTime);
  }

  UpdateCameraVectors();
  _viewMatrix = Math::LookAt(position, position + _front, _up);
}


/* -----------------------------------------------------
 *          PRIVATE METHODS
 * -----------------------------------------------------
*/

void Camera::UpdateCameraVectors()
{
  Vec3f calc_front;
  calc_front.x = Math::Cos(Math::Radians(yaw)) * cos(Math::Radians(pitch));
  calc_front.y = Math::Sin(Math::Radians(pitch));
  calc_front.z = Math::Sin(Math::Radians(yaw)) * cos(Math::Radians(pitch));
  
  _front  = Math::Normalize(calc_front);
  _right  = Math::Normalize(Math::Cross(_front, _worldUp));
  _up     = Math::Normalize(Math::Cross(_right, _front));
}

void Camera::FreeCameraWalk(const WindowManager* window, const double deltaTime)
{
  const float velocity = movementSpeed * deltaTime;

  /* W-S */
  if (window->GetKey(GLFW_KEY_W) == GLFW_PRESS)
  {
    position += _front * velocity;
  }
  else if (window->GetKey(GLFW_KEY_S) == GLFW_PRESS)
  {
    position -= _front * velocity;
  }

  /* A-D */
  if (window->GetKey(GLFW_KEY_A) == GLFW_PRESS)
  {
    position -= _right * velocity;
  }
  else if (window->GetKey(GLFW_KEY_D) == GLFW_PRESS)
  {
    position += _right * velocity;
  }

  /* SPACE-LCTRL */
  if (window->GetKey(GLFW_KEY_SPACE) == GLFW_PRESS)
  {
    position += _up * velocity;
  }
  else if (window->GetKey(GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
  {
    position -= _up * velocity;
  }
}

void Camera::FreeCameraRotation(Vec2d& mousePos, const double deltaTime)
{
  static bool firstMouse = true;
  if (firstMouse)
  {
    _lastX = mousePos.x;
    _lastY = mousePos.y;
    firstMouse = false;
  }

  const float velocity = mouseSensitivity * deltaTime * 10;

  const float xoffset = _lastX - mousePos.x;
  if (xoffset < 0) /* Right */
    yaw += velocity;
  else if (xoffset > 0) /* Left */
    yaw -= velocity;

  const float yoffset = _lastY - mousePos.y;
  if (yoffset > 0) /* Up */
    pitch += velocity;
  else if (yoffset < 0) /* Down */
    pitch -= velocity;

  _lastX = mousePos.x;
  _lastY = mousePos.y;

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
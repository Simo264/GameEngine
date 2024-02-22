#include "Camera.hpp"

#include "Engine/Window.hpp"
#include "Logger.hpp"

/* -----------------------------------------------------
 *          PUBLIC METHODS
 * -----------------------------------------------------
*/

Camera::Camera(Vec2i windowSize, Vec3f position) : position{ position }
{
  front   = Vec3f(0.0f, 0.0f, -1.0f);
  worldUp = Vec3f(0.0f, 1.0f, 0.0f);
  yaw     = -90.0f;
  pitch   = 0.0f;

  movementSpeed    = 7.5f;  /* [1.0f:10.0f] */
  mouseSensitivity = 25.0f; /* [1.0f:100.0f] */
  fov              = 45.0f;

  _lastX = ((float)windowSize.x / 2.0f);
  _lastY = ((float)windowSize.y / 2.0f);

  _firstMouse     = false;
  _constrainPitch = true;
  
  UpdateCameraVectors();
}

Mat4f Camera::GetViewMatrix() const
{
  return glm::lookAt(position, position + front, up); 
}

void Camera::ProcessInput(const Window* window, const double deltaTime)
{
  Vec2d mousePos;
  window->GetCursorPosition(mousePos);

  FreeCameraWalk(window, deltaTime);
  
  if (window->GetMouseKey(GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
  {
    FreeCameraRotation(mousePos, deltaTime);
  }
}


/* -----------------------------------------------------
 *          PRIVATE METHODS
 * -----------------------------------------------------
*/

void Camera::UpdateCameraVectors()
{
  Vec3f calcFront;
  calcFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
  calcFront.y = sin(glm::radians(pitch));
  calcFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
  
  front = glm::normalize(calcFront);
  right = glm::normalize(glm::cross(front, worldUp));  
  up    = glm::normalize(glm::cross(right, front));
}

void Camera::FreeCameraWalk(const Window* window, const double deltaTime)
{
  const float velocity = movementSpeed * deltaTime;

  /* W-S */
  if (window->GetKey(GLFW_KEY_W) == GLFW_PRESS)
  {
    position += front * velocity;
  }
  else if (window->GetKey(GLFW_KEY_S) == GLFW_PRESS)
  {
    position -= front * velocity;
  }

  /* A-D */
  if (window->GetKey(GLFW_KEY_A) == GLFW_PRESS)
  {
    position -= right * velocity;
  }
  else if (window->GetKey(GLFW_KEY_D) == GLFW_PRESS)
  {
    position += right * velocity;
  }

  /* SPACE-LCTRL */
  if (window->GetKey(GLFW_KEY_SPACE) == GLFW_PRESS)
  {
    position += up * velocity;
  }
  else if (window->GetKey(GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
  {
    position -= up * velocity;
  }
}

void Camera::FreeCameraRotation(Vec2d& mousePos, const double deltaTime)
{
  if (_firstMouse)
  {
    _lastX = mousePos.x;
    _lastY = mousePos.y;
    _firstMouse = false;
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
  if (_constrainPitch)
  {
    if (pitch > 89.0f)
      pitch = 89.0f;
    if (pitch < -89.0f)
      pitch = -89.0f;
  }

  UpdateCameraVectors();
}
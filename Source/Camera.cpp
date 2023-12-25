#include "Camera.hh"

/* -----------------------------------------------------
 *          PUBLIC METHODS
 * -----------------------------------------------------
*/

Camera::Camera(vec2i windowSize, vec3f position) : position{ position }
{
  front   = vec3f(0.0f, 0.0f, -1.0f);
  worldUp = vec3f(0.0f, 1.0f, 0.0f);
  yaw     = -90.0f;
  pitch   = 0.0f;

  movementSpeed    = 7.5f;  
  mouseSensitivity = 0.1f; 
  fov              = 45.0f;

  _lastX = (float)(windowSize.x / 2);
  _lastY = (float)(windowSize.y / 2);

  _firstMouse     = false;
  _constrainPitch = true;
  
  UpdateCameraVectors();
}

mat4f Camera::GetViewMatrix() const
{
  return glm::lookAt(position, position + front, up); 
}

void Camera::ProcessInput(Window& window, double deltaTime)
{
  vec2d mousePos;
  window.GetCursorPosition(mousePos);

  FreeCameraWalk(window, deltaTime);
  
  //if (window.GetMouseKey(GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
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
  vec3f calcFront;
  calcFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
  calcFront.y = sin(glm::radians(pitch));
  calcFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
  
  front = glm::normalize(calcFront);
  right = glm::normalize(glm::cross(front, worldUp));  
  up    = glm::normalize(glm::cross(right, front));
}

void Camera::FreeCameraWalk(const Window& window, double deltaTime)
{
  const float velocity = movementSpeed * deltaTime;

  // W-S
  if (window.GetKey(GLFW_KEY_W) == GLFW_PRESS)
  {
    position += front * velocity;
  }
  else if (window.GetKey(GLFW_KEY_S) == GLFW_PRESS)
  {
    position -= front * velocity;
  }

  // A-D
  if (window.GetKey(GLFW_KEY_A) == GLFW_PRESS)
  {
    position -= right * velocity;
  }
  else if (window.GetKey(GLFW_KEY_D) == GLFW_PRESS)
  {
    position += right * velocity;
  }

  // SPACE-LCTRL
  if (window.GetKey(GLFW_KEY_SPACE) == GLFW_PRESS)
  {
    position += up * velocity;
  }
  else if (window.GetKey(GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
  {
    position -= up * velocity;
  }
}

void Camera::FreeCameraRotation(vec2d& mousePos, double deltaTime)
{
  if (_firstMouse)
  {
    _lastX = mousePos.x;
    _lastY = mousePos.y;
    _firstMouse = false;
  }

  float xoffset = mousePos.x - _lastX;
  float yoffset = _lastY - mousePos.y; // reversed since y-coordinates go from bottom to top

  _lastX = mousePos.x;
  _lastY = mousePos.y;

  xoffset *= mouseSensitivity;
  yoffset *= mouseSensitivity;

  yaw   += xoffset;
  pitch += yoffset;

  // make sure that when pitch is out of bounds, screen doesn't get flipped
  if (_constrainPitch)
  {
    if (pitch > 89.0f)
      pitch = 89.0f;
    if (pitch < -89.0f)
      pitch = -89.0f;
  }

  UpdateCameraVectors();
}
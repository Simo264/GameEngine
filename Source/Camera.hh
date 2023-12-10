#pragma once

#include "Core.hh"
#include "Window.hh"

class Camera
{
public:
  Camera(vec3f position = vec3f(0.0f, 0.0f, 0.0f));
  ~Camera() = default;

  mat4f GetViewMatrix() const;

  void ProcessInput(Window& window, double deltaTime);

  // camera Attributes
  vec3f position;
  vec3f front;
  vec3f up;
  vec3f right;
  vec3f worldUp;

  // euler Angles
  float yaw;
  float pitch;
  
  // camera options
  float movementSpeed;
  float mouseSensitivity;
  float fov;

private:
  // calculates the front vector from the Camera's (updated) Euler Angles
  void UpdateCameraVectors();

  void FreeCameraWalk(const Window& window, double deltaTime);
  void FreeCameraRotation(vec2d& mousePos, double deltaTime);
  
  float _lastX;
  float _lastY;
  bool _firstMouse;
  bool _constrainPitch;
};

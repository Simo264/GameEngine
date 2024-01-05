#pragma once

#include "Core.hh"
#include "UncopyableObject.hh"

class Camera : public UncopyableObject
{
public:
  Camera(Vec2i windowSize, Vec3f position = Vec3f(0.0f, 0.0f, 0.0f));

  Mat4f GetViewMatrix() const;

  void ProcessInput(class Window* window, double deltaTime);

  // camera Attributes
  Vec3f position;
  Vec3f front;
  Vec3f up;
  Vec3f right;
  Vec3f worldUp;

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

  void FreeCameraWalk(const class Window* window, double deltaTime);
  void FreeCameraRotation(Vec2d& mousePos, double deltaTime);
  
  float _lastX;
  float _lastY;
  bool _firstMouse;
  bool _constrainPitch;
};

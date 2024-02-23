#pragma once

#include "Core.hpp"

/* --------------------------------------- 
      Camera class 
  --------------------------------------- */
class Camera
{
public:
  Camera(Vec3f position, float fov);

  const Mat4f& GetViewMatrix() const{ return _viewMatrix; }

  void ProcessInput(const double deltaTime);

  /* Camera Attributes */
  Vec3f position;
  float fov;

  /* Euler Angles */
  float yaw;
  float pitch;
  
  /* Camera options */
  float movementSpeed;
  float mouseSensitivity;

private:
  /* Calculates the front vector from the Camera's (updated) Euler Angles */
  void UpdateCameraVectors();

  void FreeCameraWalk(const class Window* window, const double deltaTime);
  void FreeCameraRotation(Vec2d& mousePos, const double deltaTime);
  
  Mat4f _viewMatrix;

  /* Camera vectors */
  Vec3f _front;
  Vec3f _up;
  Vec3f _right;
  Vec3f _worldUp;

  float _lastX;
  float _lastY;
};

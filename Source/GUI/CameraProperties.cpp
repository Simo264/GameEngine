#include "Engine/Camera.hpp"

#include <imgui.h>

void Gui_RenderCameraSettings(Camera& camera)
{
  ImGui::Begin("Camera settings");

  // Position
  ImGui::Text("Position");
  ImGui::SliderFloat("X", &camera.position.x, -100.0f, 100.0f);
  ImGui::SliderFloat("Y", &camera.position.y, -100.0f, 100.0f);
  ImGui::SliderFloat("Z", &camera.position.z, -100.0f, 100.0f);

  // Orientation
  ImGui::Text("Orientation");
  ImGui::SliderFloat("Yaw", &camera.orientation.x,    -180.0f, 180.0f);
  ImGui::SliderFloat("Pitch", &camera.orientation.y,  -180.0f, 180.0f);
  ImGui::SliderFloat("Roll", &camera.orientation.z,   -180.0f, 180.0f);

  // Field of View
  ImGui::Text("Field of View");
  ImGui::SliderFloat("FOV", &camera.fov, 1.0f, 120.0f);

  // Near and Far Planes
  ImGui::Text("Frustum Settings");
  ImGui::SliderFloat("Left", &camera.frustum.left, -50.0f, 50.0f);
  ImGui::SliderFloat("Right", &camera.frustum.right, -50.0f, 50.0f);
  ImGui::SliderFloat("Bottom", &camera.frustum.bottom, -50.0f, 50.0f);
  ImGui::SliderFloat("Top", &camera.frustum.top, -50.0f, 50.0f);
  ImGui::SliderFloat("Near Plane", &camera.frustum.zNear, 0.01f, 10.0f);
  ImGui::SliderFloat("Far Plane", &camera.frustum.zFar, 10.0f, 100.0f);

  camera.UpdateOrientation();

  ImGui::End();
}
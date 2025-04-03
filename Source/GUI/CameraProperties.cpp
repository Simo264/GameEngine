#include "Engine/Camera.hpp"

#include <imgui.h>

void Gui_RenderCameraProps(Camera& camera)
{
  ImGui::Begin("Camera Settings");

  // Position
  ImGui::Text("Position");
  ImGui::SliderFloat("X", &camera.position.x, -100.0f, 100.0f);
  ImGui::SliderFloat("Y", &camera.position.y, -100.0f, 100.0f);
  ImGui::SliderFloat("Z", &camera.position.z, -100.0f, 100.0f);

  // Orientation
  ImGui::Text("Orientation");
  ImGui::SliderFloat("Yaw", &camera.orientation.x, -180.0f, 180.0f);
  ImGui::SliderFloat("Pitch", &camera.orientation.y, -90.0f, 90.0f);
  ImGui::SliderFloat("Roll", &camera.orientation.z, -180.0f, 180.0f);

  // Field of View
  ImGui::Text("Field of View");
  ImGui::SliderFloat("FOV", &camera.fov, 1.0f, 120.0f);

  camera.UpdateOrientation();

  ImGui::End();
}
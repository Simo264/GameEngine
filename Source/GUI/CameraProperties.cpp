//#include "Core/Core.hpp"
//#include "Engine/Camera.hpp"
//#include <imgui/imgui.h>
//
//void GUI_RenderCameraProperties(bool& open, Camera& camera)
//{
//  ImGui::Begin("Camera properties", &open);
//
//  ImGui::DragFloat("zNear", &camera.frustum.zNear, 0.1f, 0.0f, 1'000.0f);
//  ImGui::DragFloat("zFar", &camera.frustum.zFar, 0.1f, 0.0f, 1'000.0f);
//  ImGui::DragFloat("Left", &camera.frustum.left, 0.1f, -100.0f, 0.0f);
//  ImGui::DragFloat("Right", &camera.frustum.right, 0.1f, 0.0f, 100.0f);
//  ImGui::DragFloat("Top", &camera.frustum.top, 0.1f, 0.0f, 100.0f);
//  ImGui::DragFloat("Bottom", &camera.frustum.bottom, 0.1f, -100.0f, 0.0);
//
//  ImGui::DragFloat("Yaw", &camera.yaw, 0.1f, -360.0f, 360);
//  ImGui::DragFloat("Pitch", &camera.pitch, 0.1f, -360, 360);
//  ImGui::DragFloat("Roll", &camera.roll, 0.1f, -360, 360.0f);
//  ImGui::DragFloat3("Position", (f32*)&camera.position, 0.1f, -FLT_MAX, FLT_MAX);
//
//  ImGui::DragFloat("Fov", &camera.fov, 0.1f, 0.0f, 180.0f);
//
//  ImGui::End();
//}

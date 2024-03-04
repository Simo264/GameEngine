#include "DetailsPanel.hpp"
#include "Core/Log/Logger.hpp"
#include "Engine/GameObject.hpp"

#include "Engine/ECS/TransformComponent.hpp"
#include "Engine/ECS/StaticMeshComponent.hpp"
#include "Engine/ECS/Lighting/DirLightComponent.hpp"
#include "Engine/ECS/Lighting/PointLightComponent.hpp"

#include <imgui/imgui.h>

/* -----------------------------------------------------
 *          PUBLIC METHODS
 * -----------------------------------------------------
*/

#if 0
void DetailsPanel::RenderPanel(SharedPointer<DirectionalLight>& target, Scene* scene)
{
  ImGui::Begin(panelName.c_str(), &isOpen);
  if (ImGui::CollapsingHeader("Light properties"))
  {
    ImGui::BeginTable("##table", 2);
    float C1 = 0.30f * ImGui::GetContentRegionAvail().x;
    float C2 = 0.70f * ImGui::GetContentRegionAvail().x;
    ImGui::TableSetupColumn("##C1", ImGuiTableColumnFlags_WidthFixed, C1);
    ImGui::TableSetupColumn("##C2", ImGuiTableColumnFlags_WidthFixed, C2);

    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    ImGui::Text("Light color");
    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(-1);
    ImGui::ColorEdit3("##color", (float*)&target->color);

    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    ImGui::Text("Ambient");
    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(-1);
    ImGui::SliderFloat("##ambient", &target->ambient, 0.0f, 1.0f);

    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    ImGui::Text("Diffuse");
    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(-1);
    ImGui::SliderFloat("##diffuse", &target->diffuse, 0.0f, 1.0f);

    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    ImGui::Text("Specular");
    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(-1);
    ImGui::SliderFloat("##specular", &target->specular, 0.0f, 1.0f);

    ImGui::TableNextRow();
    EditVec3("Direction", 1, { -FLT_MAX, +FLT_MAX }, target->direction, C2);

    ImGui::EndTable();
  }
  if (ImGui::CollapsingHeader("Advanced"))
  {
    if (ImGui::Button("Delete"))
      scene->RemoveSceneObject(target);
  }
  ImGui::End();
}

void DetailsPanel::RenderPanel(SharedPointer<PointLight>& target, Scene* scene)
{
  if (!isOpen)
    return;

  ImGui::Begin(panelName.c_str(), &isOpen);
  if (ImGui::CollapsingHeader("Light properties"))
  {
    ImGui::BeginTable("##table", 2);
    int C1 = 0.30f * ImGui::GetContentRegionAvail().x;
    int C2 = 0.70f * ImGui::GetContentRegionAvail().x;
    ImGui::TableSetupColumn("##C1", ImGuiTableColumnFlags_WidthFixed, C1);
    ImGui::TableSetupColumn("##C2", ImGuiTableColumnFlags_WidthFixed, C2);

    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    ImGui::Text("Light color");
    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(-1);
    ImGui::ColorEdit3("##color", (float*)&target->color);

    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    ImGui::Text("Ambient");
    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(-1);
    ImGui::SliderFloat("##ambient", &target->ambient, 0.0f, 1.0f);

    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    ImGui::Text("Diffuse");
    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(-1);
    ImGui::SliderFloat("##diffuse", &target->diffuse, 0.0f, 1.0f);

    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    ImGui::Text("Specular");
    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(-1);
    ImGui::SliderFloat("##specular", &target->specular, 0.0f, 1.0f);

    ImGui::TableNextRow();
    EditVec3("Position", 1, { -FLT_MAX, +FLT_MAX }, target->position, C2);

    ImGui::EndTable();
  }
  if (ImGui::CollapsingHeader("Advanced"))
  {
    if (ImGui::Button("Delete"))
      scene->RemoveSceneObject(target);
  }
  
  ImGui::End();
}

void DetailsPanel::RenderPanel(SharedPointer<StaticMesh>& target, Scene* scene)
{
  if (!isOpen)
    return;

  ImGui::Begin(panelName.c_str(), &isOpen);
  if (ImGui::CollapsingHeader("Transform"))
  {
    ImGui::BeginTable("##table", 2);
    int C1 = 0.30f * ImGui::GetContentRegionAvail().x;
    int C2 = 0.70f * ImGui::GetContentRegionAvail().x;
    ImGui::TableSetupColumn("##C1", ImGuiTableColumnFlags_WidthFixed, C1);
    ImGui::TableSetupColumn("##C2", ImGuiTableColumnFlags_WidthFixed, C2);

    ImGui::TableNextRow();
    EditVec3("Position", 1, { -FLT_MAX, +FLT_MAX }, target->transform.position, C2);

    ImGui::TableNextRow();
    EditVec3("Scale", 1, { -FLT_MAX, +FLT_MAX }, target->transform.scale, C2);

    ImGui::TableNextRow();
    EditVec3("Rotate", 1, { -180.0f, 180.0f}, target->transform.degrees, C2);

    ImGui::EndTable();
  }
  if (ImGui::CollapsingHeader("Material"))
  {
    const String diffusePathStr = (
      target->material.diffuse ? target->material.diffuse->texturePath.string() : "");
    const uint32_t diffuseID = (
      target->material.diffuse ? target->material.diffuse->textureID : 0);

    ImGui::BeginTable("##table", 3, ImGuiTableFlags_BordersInnerH);
    ImGui::TableSetupColumn("##C1", ImGuiTableColumnFlags_WidthFixed, 0.5f * ImGui::GetContentRegionAvail().x);
    ImGui::TableSetupColumn("##C2", ImGuiTableColumnFlags_WidthFixed, 0.3f * ImGui::GetContentRegionAvail().x);
    ImGui::TableSetupColumn("##C3", ImGuiTableColumnFlags_WidthFixed, 0.2f * ImGui::GetContentRegionAvail().x);

    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    ImGui::Image((ImTextureID)diffuseID, { 64,64 });

    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(-1);
    if (ImGui::BeginCombo("##Textures", diffusePathStr.c_str()))
    {
      auto& instanceTM = TextureManager::Instance();
      std::for_each(instanceTM.Begin(), instanceTM.End(), [&](Texture2D& texture) {
        String textPathStr = texture.texturePath.string();
        bool isSelected = (std::strcmp(diffusePathStr.c_str(), textPathStr.c_str()) == 0);
        if (ImGui::Selectable(textPathStr.c_str(), isSelected))
          target->material.diffuse = instanceTM.GetTextureByPath(textPathStr.c_str());
        if (isSelected)
          ImGui::SetItemDefaultFocus();
        });

      ImGui::EndCombo();
    }

    ImGui::TableNextColumn();
    ImGui::Button("reset");

    ImGui::EndTable();
  }
  if (ImGui::CollapsingHeader("Advanced"))
  {
    if (ImGui::Button("Delete"))
    {
      scene->RemoveSceneObject(target);
    }
  }

  ImGui::End();
}
#endif

void DetailsPanel::RenderPanel(GameObject& object)
{
  ImGui::Begin(panelName.c_str(), &visible);
  if (object.IsValid())
  {
    if (auto dLightComp = object.GetComponent<DirLightComponent>())
    {
      if (ImGui::CollapsingHeader("DirLightComponent properties"))
      {

      }
    }
    if (auto pLightComp = object.GetComponent<PointLightComponent>())
    {
      if (ImGui::CollapsingHeader("PointLightComponent properties"))
      {

      }
    }
    if (auto transComp = object.GetComponent<TransformComponent>())
    {
      if (ImGui::CollapsingHeader("Transform properties"))
      {

      }
    }
    if (auto meshComp = object.GetComponent<StaticMeshComponent>())
    {
      if (ImGui::CollapsingHeader("Mesh properties"))
      {

      }
    }
  }
  ImGui::End();
}

/* -----------------------------------------------------
 *          PRIVATE METHODS
 * -----------------------------------------------------
*/

void DetailsPanel::EditVec3(const char* label, float speed, Vec2f minMax, Vec3f& values, float colSize)
{
  ImGui::PushID(label);

  ImGui::TableNextColumn();
  ImGui::Text(label);
  ImGui::TableNextColumn();
  ImGui::SetNextItemWidth((colSize / 3.0f) - 5);
  ImGui::DragFloat("##X", &values.x, speed, minMax[0], minMax[1], "X: %.2f");
  ImGui::SameLine(0, 5);
  ImGui::SetNextItemWidth((colSize / 3.0f) - 5);
  ImGui::DragFloat("##Y", &values.y, speed, minMax[0], minMax[1], "Y: %.2f");
  ImGui::SameLine(0, 5);
  ImGui::SetNextItemWidth((colSize / 3.0f) - 5);
  ImGui::DragFloat("##Z", &values.z, speed, minMax[0], minMax[1], "Z: %.2f");

  ImGui::PopID();
}

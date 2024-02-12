#include "DetailsPanel.hh"

#include "../Lighting/DirectionalLight.hh"
#include "../Lighting/PointLight.hh"
#include "../Lighting/SpotLight.hh"
#include "../Mesh/StaticMesh.hh"

#include "../Subsystems/TexturesManager.hh"

#include "../Logger.hh"

#include "Imgui/imgui.h"
#include "Imgui/imgui_impl_glfw.h"
#include "Imgui/imgui_impl_opengl3.h"
#include "Imgui/imgui_stdlib.h"
#include "Imgui/imgui_internal.h"
#include "Imgui/imgui_spectrum.h"

DetailsPanel::DetailsPanel(const char* panelName)
{
	this->panelName = panelName;
}

template<class T>
void DetailsPanel::RenderPanel(SceneObject<T>& sceneObject) {}

template<>
void DetailsPanel::RenderPanel(SceneObject<DirectionalLight>& sceneObject)
{
  if (!isOpen)
    return;

  ImGui::Begin(panelName.c_str(), &isOpen);
  ImGui::SeparatorText(sceneObject.name.c_str());

  auto obj = sceneObject.object;
  ImGui::ColorEdit3("Color", (float*)&obj->color);
  ImGui::SliderFloat("Ambient", &obj->ambient, 0.0f, 1.0f);
  ImGui::SliderFloat("Diffuse", &obj->diffuse, 0.0f, 1.0f);
  ImGui::SliderFloat("Specular", &obj->specular, 0.0f, 1.0f);
  ImGui::DragFloat3("Direction", (float*)&obj->direction, 0.1f, -FLT_MAX, +FLT_MAX);
  ImGui::End();
}

template<>
void DetailsPanel::RenderPanel(SceneObject<PointLight>& sceneObject)
{
  if (!isOpen)
    return;

  ImGui::Begin(panelName.c_str(), &isOpen);
  ImGui::SeparatorText(sceneObject.name.c_str());

  auto obj = sceneObject.object;
  ImGui::ColorEdit3("Color", (float*)&obj->color);
  ImGui::SliderFloat("Ambient", &obj->ambient, 0.0f, 1.0f);
  ImGui::SliderFloat("Diffuse", &obj->diffuse, 0.0f, 1.0f);
  ImGui::SliderFloat("Specular", &obj->specular, 0.0f, 1.0f);
  ImGui::DragFloat3("Position", (float*)&obj->position, 0.1f, -FLT_MAX, +FLT_MAX);
  ImGui::End();
}

template<>
void DetailsPanel::RenderPanel(SceneObject<StaticMesh>& sceneObject)
{
  ImGui::Begin(panelName.c_str(), &isOpen);

  auto obj = sceneObject.object;
  if (ImGui::CollapsingHeader("Transform"))
  {
    auto& style = ImGui::GetStyle();
    auto tmp = style.CellPadding;
    style.CellPadding = ImVec2(5.0f, 5.0f);

    ImGui::BeginTable("##table", 2, ImGuiTableFlags_BordersInnerH);

    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    ImGui::AlignTextToFramePadding();
    ImGui::Text("Translation");
    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(-1);
    ImGui::DragFloat("##translate-X", &obj->position.x, 0.1f, -FLT_MAX, +FLT_MAX, "X: %.3f");
    ImGui::SetNextItemWidth(-1);
    ImGui::DragFloat("##translate-Y", &obj->position.y, 0.1f, -FLT_MAX, +FLT_MAX, "Y: %.3f");
    ImGui::SetNextItemWidth(-1);
    ImGui::DragFloat("##translate-Z", &obj->position.z, 0.1f, -FLT_MAX, +FLT_MAX, "Z: %.3f");
    
    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    ImGui::AlignTextToFramePadding();
    ImGui::Text("Scaling");
    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(-1);
    ImGui::DragFloat("##Scaling-X", &obj->scaling.x, 0.1f, -FLT_MAX, +FLT_MAX, "X: %.3f");
    ImGui::SetNextItemWidth(-1);
    ImGui::DragFloat("##Scaling-Y", &obj->scaling.y, 0.1f, -FLT_MAX, +FLT_MAX, "Y: %.3f");
    ImGui::SetNextItemWidth(-1);
    ImGui::DragFloat("##Scaling-Z", &obj->scaling.z, 0.1f, -FLT_MAX, +FLT_MAX, "Z: %.3f");

    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    ImGui::AlignTextToFramePadding();
    ImGui::Text("Rotation");
    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(-1);
    ImGui::DragFloat("##Roll", &obj->rotationAxis.x, 1.0f, 0.0f, 1.0f, "X: %.3f");
    ImGui::SetNextItemWidth(-1);
    ImGui::DragFloat("##Pitch", &obj->rotationAxis.y, 1.0f, 0.0f, 1.0f, "Y: %.3f");
    ImGui::SetNextItemWidth(-1);
    ImGui::DragFloat("##Yaw", &obj->rotationAxis.z, 1.0f, 0.0f, 1.0f, "Z: %.3f");
    ImGui::SetNextItemWidth(-1);
    ImGui::DragFloat("##Rotation-degree", &obj->rotationAngle, 0.5f, -180.0f, +180.0f, "Degree: %.3f");

    style.CellPadding = tmp;

    ImGui::EndTable();
  }
  if (ImGui::CollapsingHeader("Material"))
  {
    const String diffusePathStr = (obj->diffuse ? obj->diffuse->texturePath.string() : "");
    const uint32_t diffuseID = (obj->diffuse ? obj->diffuse->textureID : 0);

    ImGui::BeginTable("##table", 3, ImGuiTableFlags_BordersInnerH);
    ImGui::TableSetupColumn("##C1", ImGuiTableColumnFlags_WidthFixed, 0.5f * ImGui::GetContentRegionAvail().x);   /* 40% width */
    ImGui::TableSetupColumn("##C2", ImGuiTableColumnFlags_WidthFixed, 0.3f * ImGui::GetContentRegionAvail().x);   /* 40% width */
    ImGui::TableSetupColumn("##C3", ImGuiTableColumnFlags_WidthFixed, 0.2f * ImGui::GetContentRegionAvail().x);   /* 20% width */

    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    ImGui::Image((ImTextureID)diffuseID, { 64,64 });

    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(-1);
    if (ImGui::BeginCombo("##Textures", diffusePathStr.c_str()))
    {
      auto& instanceTM = TexturesManager::Instance();
      std::for_each(instanceTM.Begin(), instanceTM.End(), [&](Texture2D& texture) {
        String textPathStr = texture.texturePath.string();
        bool isSelected = (std::strcmp(diffusePathStr.c_str(), textPathStr.c_str()) == 0);
        if (ImGui::Selectable(textPathStr.c_str(), isSelected))
          obj->diffuse = instanceTM.GetTextureByPath(textPathStr.c_str());
        if (isSelected)
          ImGui::SetItemDefaultFocus();
        });

      ImGui::EndCombo();
    }

    ImGui::TableNextColumn();
    ImGui::Button("reset");

    ImGui::EndTable();
  }

  ImGui::End();
}


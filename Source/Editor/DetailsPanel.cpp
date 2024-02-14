#include "DetailsPanel.hh"

#include "../SceneObject.hh"

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

/* -----------------------------------------------------
 *          PUBLIC METHODS
 * -----------------------------------------------------
*/

/* Directional light details */
template<>
void DetailsPanel::RenderPanel<DirectionalLight>(SceneObject* sceneObject)
{
  if (!isOpen)
    return;

  DirectionalLight* dLight = static_cast<DirectionalLight*>(sceneObject);

  ImGui::Begin(panelName.c_str(), &isOpen);
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
  ImGui::ColorEdit3("##color", (float*)&dLight->color);

  ImGui::TableNextRow();
  ImGui::TableNextColumn();
  ImGui::Text("Ambient");
  ImGui::TableNextColumn();
  ImGui::SetNextItemWidth(-1);
  ImGui::SliderFloat("##ambient", &dLight->ambient, 0.0f, 1.0f);
  
  ImGui::TableNextRow();
  ImGui::TableNextColumn();
  ImGui::Text("Diffuse");
  ImGui::TableNextColumn();
  ImGui::SetNextItemWidth(-1);
  ImGui::SliderFloat("##diffuse", &dLight->diffuse, 0.0f, 1.0f);

  ImGui::TableNextRow();
  ImGui::TableNextColumn();
  ImGui::Text("Specular");
  ImGui::TableNextColumn();
  ImGui::SetNextItemWidth(-1);
  ImGui::SliderFloat("##specular", &dLight->specular, 0.0f, 1.0f);

  ImGui::TableNextRow();
  EditVec3("Direction", 1, { -FLT_MAX, +FLT_MAX }, dLight->direction, C2);

  ImGui::EndTable();
  ImGui::End();
}


/* Point light details */
template<>
void DetailsPanel::RenderPanel<PointLight>(SceneObject* sceneObject)
{
  if (!isOpen)
    return;

  PointLight* pLight = static_cast<PointLight*>(sceneObject);

  ImGui::Begin(panelName.c_str(), &isOpen);
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
  ImGui::ColorEdit3("##", (float*)&pLight->color);

  ImGui::TableNextRow();
  ImGui::TableNextColumn();
  ImGui::Text("Ambient");
  ImGui::TableNextColumn();
  ImGui::SetNextItemWidth(-1);
  ImGui::SliderFloat("##", &pLight->ambient, 0.0f, 1.0f);

  ImGui::TableNextRow();
  ImGui::TableNextColumn();
  ImGui::Text("Diffuse");
  ImGui::TableNextColumn();
  ImGui::SetNextItemWidth(-1);
  ImGui::SliderFloat("##", &pLight->diffuse, 0.0f, 1.0f);

  ImGui::TableNextRow();
  ImGui::TableNextColumn();
  ImGui::Text("Specular");
  ImGui::TableNextColumn();
  ImGui::SetNextItemWidth(-1);
  ImGui::SliderFloat("##", &pLight->specular, 0.0f, 1.0f);

  ImGui::TableNextRow();
  EditVec3("Position", 1, { -FLT_MAX, +FLT_MAX }, pLight->position, C2);

  ImGui::EndTable();
  ImGui::End();
}


/* Static mesh details */
template<>
void DetailsPanel::RenderPanel<StaticMesh>(SceneObject* sceneObject)
{
  if (!isOpen)
    return;

  StaticMesh* sMesh = static_cast<StaticMesh*>(sceneObject);
  
  ImGui::Begin(panelName.c_str(), &isOpen);
  if (ImGui::CollapsingHeader("Transform"))
  {
    ImGui::BeginTable("##table", 2);
    int C1 = 0.30f * ImGui::GetContentRegionAvail().x;
    int C2 = 0.70f * ImGui::GetContentRegionAvail().x;
    ImGui::TableSetupColumn("##C1", ImGuiTableColumnFlags_WidthFixed, C1);
    ImGui::TableSetupColumn("##C2", ImGuiTableColumnFlags_WidthFixed, C2);

    ImGui::TableNextRow();
    EditVec3("Position", 1, { -FLT_MAX, +FLT_MAX }, sMesh->transform.position, C2);

    ImGui::TableNextRow();
    EditVec3("Scale", 1, { -FLT_MAX, +FLT_MAX }, sMesh->transform.scale, C2);

    ImGui::TableNextRow();
    EditVec3("Rotate", 1, { -180.0f, 180.0f}, sMesh->transform.degrees, C2);

    ImGui::EndTable();
  }
  if (ImGui::CollapsingHeader("Material"))
  {
    const String diffusePathStr = (sMesh->diffuse ? sMesh->diffuse->texturePath.string() : "");
    const uint32_t diffuseID = (sMesh->diffuse ? sMesh->diffuse->textureID : 0);

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
      auto& instanceTM = TexturesManager::Instance();
      std::for_each(instanceTM.Begin(), instanceTM.End(), [&](Texture2D& texture) {
        String textPathStr = texture.texturePath.string();
        bool isSelected = (std::strcmp(diffusePathStr.c_str(), textPathStr.c_str()) == 0);
        if (ImGui::Selectable(textPathStr.c_str(), isSelected))
          sMesh->diffuse = instanceTM.GetTextureByPath(textPathStr.c_str());
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

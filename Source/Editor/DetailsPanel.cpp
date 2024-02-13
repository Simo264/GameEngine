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

/* -----------------------------------------------------
 *          PUBLIC METHODS
 * -----------------------------------------------------
*/

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

  auto obj = sceneObject.object;

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
  ImGui::ColorEdit3("##color", (float*)&obj->color);

  ImGui::TableNextRow();
  ImGui::TableNextColumn();
  ImGui::Text("Ambient");
  ImGui::TableNextColumn();
  ImGui::SetNextItemWidth(-1);
  ImGui::SliderFloat("##ambient", &obj->ambient, 0.0f, 1.0f);
  
  ImGui::TableNextRow();
  ImGui::TableNextColumn();
  ImGui::Text("Diffuse");
  ImGui::TableNextColumn();
  ImGui::SetNextItemWidth(-1);
  ImGui::SliderFloat("##diffuse", &obj->diffuse, 0.0f, 1.0f);

  ImGui::TableNextRow();
  ImGui::TableNextColumn();
  ImGui::Text("Specular");
  ImGui::TableNextColumn();
  ImGui::SetNextItemWidth(-1);
  ImGui::SliderFloat("##specular", &obj->specular, 0.0f, 1.0f);

  ImGui::TableNextRow();
  EditVec3("Direction", 1, { -FLT_MAX, +FLT_MAX }, { &obj->direction.x,&obj->direction.y,&obj->direction.z }, C2);

  ImGui::EndTable();
  ImGui::End();
}

template<>
void DetailsPanel::RenderPanel(SceneObject<PointLight>& sceneObject)
{
  if (!isOpen)
    return;
  
  auto obj = sceneObject.object;

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
  ImGui::ColorEdit3("##", (float*)&obj->color);

  ImGui::TableNextRow();
  ImGui::TableNextColumn();
  ImGui::Text("Ambient");
  ImGui::TableNextColumn();
  ImGui::SetNextItemWidth(-1);
  ImGui::SliderFloat("##", &obj->ambient, 0.0f, 1.0f);

  ImGui::TableNextRow();
  ImGui::TableNextColumn();
  ImGui::Text("Diffuse");
  ImGui::TableNextColumn();
  ImGui::SetNextItemWidth(-1);
  ImGui::SliderFloat("##", &obj->diffuse, 0.0f, 1.0f);

  ImGui::TableNextRow();
  ImGui::TableNextColumn();
  ImGui::Text("Specular");
  ImGui::TableNextColumn();
  ImGui::SetNextItemWidth(-1);
  ImGui::SliderFloat("##", &obj->specular, 0.0f, 1.0f);

  ImGui::TableNextRow();
  EditVec3("Position", 1, { -FLT_MAX, +FLT_MAX }, { &obj->position.x,&obj->position.y,&obj->position.z }, C2);

  ImGui::EndTable();
  ImGui::End();
}

template<>
void DetailsPanel::RenderPanel(SceneObject<StaticMesh>& sceneObject)
{
  ImGui::Begin(panelName.c_str(), &isOpen);

  auto obj = sceneObject.object;
  if (ImGui::CollapsingHeader("Transform"))
  {
    ImGui::BeginTable("##table", 2);
    int C1 = 0.30f * ImGui::GetContentRegionAvail().x;
    int C2 = 0.70f * ImGui::GetContentRegionAvail().x;
    ImGui::TableSetupColumn("##C1", ImGuiTableColumnFlags_WidthFixed, C1);
    ImGui::TableSetupColumn("##C2", ImGuiTableColumnFlags_WidthFixed, C2);

    ImGui::TableNextRow();
    EditVec3("Position", 1, { -FLT_MAX, +FLT_MAX }, { &obj->position.x,&obj->position.y,&obj->position.z }, C2);

    ImGui::TableNextRow();
    EditVec3("Scale", 1, { -FLT_MAX, +FLT_MAX }, { &obj->scaling.x,&obj->scaling.y,&obj->scaling.z }, C2);

    ImGui::TableNextRow();
    EditVec3("Rotate", 1, { -180.0f, 180.0f}, { &obj->angleDegreeX,&obj->angleDegreeY,&obj->angleDegreeZ }, C2);

    ImGui::EndTable();
  }
  if (ImGui::CollapsingHeader("Material"))
  {
    const String diffusePathStr = (obj->diffuse ? obj->diffuse->texturePath.string() : "");
    const uint32_t diffuseID = (obj->diffuse ? obj->diffuse->textureID : 0);

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


/* -----------------------------------------------------
 *          PRIVATE METHODS
 * -----------------------------------------------------
*/

void DetailsPanel::EditVec3(const char* label, float speed, Vec2f minMax, Array<float*, 3> coords, float colSize)
{
  ImGui::PushID(label);

  ImGui::TableNextColumn();
  ImGui::Text(label);
  ImGui::TableNextColumn();
  ImGui::SetNextItemWidth((colSize / 3.0f) - 5);
  ImGui::DragFloat("##X", coords[0], speed, minMax[0], minMax[1], "X: %.2f");
  ImGui::SameLine(0, 5);
  ImGui::SetNextItemWidth((colSize / 3.0f) - 5);
  ImGui::DragFloat("##Y", coords[1], speed, minMax[0], minMax[1], "Y: %.2f");
  ImGui::SameLine(0, 5);
  ImGui::SetNextItemWidth((colSize / 3.0f) - 5);
  ImGui::DragFloat("##Z", coords[2], speed, minMax[0], minMax[1], "Z: %.2f");

  ImGui::PopID();
}

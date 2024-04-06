#include "DetailsPanel.hpp"
#include "Core/Log/Logger.hpp"

#include "Engine/Scene.hpp"

#include "Engine/GameObject.hpp"
#include "Engine/Components.hpp"

#include "Engine/Graphics/Texture2D.hpp"
#include "Engine/Subsystems/TextureManager.hpp"

#include <imgui/imgui.h>
#include <imgui/ImGuizmo.h>

/* -----------------------------------------------------
 *          PUBLIC METHODS
 * -----------------------------------------------------
*/

DetailsPanel::DetailsPanel(const char* panelName, bool visible)
  : Panel(panelName, visible),
  _grizmoMode{ static_cast<int>(ImGuizmo::OPERATION::TRANSLATE) }
{}

void DetailsPanel::RenderPanel(Scene& scene, GameObject& selected)
{
  ImGui::Begin(panelName.c_str(), &visible);
  if (selected.IsValid())
  {
    /* Directional light component */
    if (auto dLightComp = selected.GetComponent<DirLightComponent>())
    {
      if (ImGui::CollapsingHeader("Light properties", ImGuiTreeNodeFlags_DefaultOpen))
      {
        const float C1 = 0.30f * ImGui::GetContentRegionAvail().x;
        const float C2 = 0.70f * ImGui::GetContentRegionAvail().x;
        CreateTable<2>("##LightProps", { C1, C2 });
        LightProperties(dLightComp);
        EndTable();
      }
      if (ImGui::CollapsingHeader("Directional light properties", ImGuiTreeNodeFlags_DefaultOpen))
      {
        const float C1 = 0.30f * ImGui::GetContentRegionAvail().x;
        const float C2 = 0.70f * ImGui::GetContentRegionAvail().x;
        CreateTable<2>("##DirectionalLightProps", { C1, C2 });
        ImGui::TableNextRow();
        EditVec3("Direction", 0.1f, { -FLT_MAX, +FLT_MAX }, dLightComp->direction, C2);
        EndTable();
      }
    }
    
    /* Point light component */
    else if (auto pLightComp = selected.GetComponent<PointLightComponent>())
    {
      if (ImGui::CollapsingHeader("Light properties", ImGuiTreeNodeFlags_DefaultOpen))
      {
        const float C1 = 0.30f * ImGui::GetContentRegionAvail().x;
        const float C2 = 0.70f * ImGui::GetContentRegionAvail().x;
        CreateTable<2>("##LighProps", { C1, C2 });
        LightProperties(pLightComp);
        EndTable();
      }
      if (ImGui::CollapsingHeader("Point light properties", ImGuiTreeNodeFlags_DefaultOpen))
      {
        const float C1 = 0.30f * ImGui::GetContentRegionAvail().x;
        const float C2 = 0.70f * ImGui::GetContentRegionAvail().x;
        CreateTable<2>("##PointLightProps", { C1, C2 });
        ImGui::TableNextRow();
        EditVec3("Position", 0.1f, { -FLT_MAX, +FLT_MAX }, pLightComp->position, C2);
        EndTable();
      }
    }

    /* Spot light component */
    else if (auto sLightComp = selected.GetComponent<SpotLightComponent>())
    {
      /* TODO */
    }

    /* Transformation component */
    else if (auto transComp = selected.GetComponent<TransformComponent>())
    {
      if (ImGui::CollapsingHeader("Transformation", ImGuiTreeNodeFlags_DefaultOpen))
      {
        if (ImGui::RadioButton("Translate", _grizmoMode == static_cast<int>(ImGuizmo::OPERATION::TRANSLATE)))
          _grizmoMode = static_cast<int>(ImGuizmo::OPERATION::TRANSLATE);
        ImGui::SameLine();
        if (ImGui::RadioButton("Rotate", _grizmoMode == static_cast<int>(ImGuizmo::OPERATION::ROTATE)))
          _grizmoMode = static_cast<int>(ImGuizmo::OPERATION::ROTATE);
        ImGui::SameLine();
        if (ImGui::RadioButton("Scale", _grizmoMode == static_cast<int>(ImGuizmo::OPERATION::SCALE)))
          _grizmoMode = static_cast<int>(ImGuizmo::OPERATION::SCALE);
        ImGui::Separator();

        const float C1 = 0.30f * ImGui::GetContentRegionAvail().x;
        const float C2 = 0.70f * ImGui::GetContentRegionAvail().x;
        CreateTable<2>("##Transformation", { C1, C2 });
        
        ImGui::TableNextRow();
        EditVec3("Position", 0.1f, { -FLT_MAX, +FLT_MAX }, transComp->position, C2);
        ImGui::TableNextRow();
        EditVec3("Scale", 0.1f, { -FLT_MAX, +FLT_MAX }, transComp->scale, C2);
        ImGui::TableNextRow();
        EditVec3("Rotate (deg)", 1.0f, { -180.0f, +180.0f }, transComp->rotation, C2);
        
        transComp->UpdateTransformation();

        EndTable();
      }
    }
    
    /* Static mesh component */
    if (auto meshComp = selected.GetComponent<StaticMeshComponent>())
    {
      if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen))
      {
        const float C1 = 0.5f * ImGui::GetContentRegionAvail().x;
        const float C2 = 0.3f * ImGui::GetContentRegionAvail().x;
        const float C3 = 0.2f * ImGui::GetContentRegionAvail().x;
        CreateTable<3>("##Material", { C1, C2, C3 });

        const String diffusePathStr = 
          (meshComp->material.diffuse ? meshComp->material.diffuse->texturePath.string() : "");
        const uint32_t diffuseID = 
          (meshComp->material.diffuse ? meshComp->material.diffuse->textureID : 0xFFFFFFFF);

        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Image(reinterpret_cast<ImTextureID>(diffuseID), { 64,64 });

        ImGui::TableNextColumn();
        ImGui::SetNextItemWidth(-1);
        if (ImGui::BeginCombo("##Textures", diffusePathStr.c_str()))
        {
          auto& instanceTM = TextureManager::Instance();
          std::for_each(instanceTM.Begin(), instanceTM.End(), [&](Texture2D* texture) {
            String textPathStr = texture->texturePath.string();
            bool isSelected = (std::strcmp(diffusePathStr.c_str(), textPathStr.c_str()) == 0);
            if (ImGui::Selectable(textPathStr.c_str(), isSelected))
              meshComp->material.diffuse = &instanceTM.GetTextureByPath(textPathStr.c_str());
            if (isSelected)
              ImGui::SetItemDefaultFocus();
            });

          ImGui::EndCombo();
        }

        ImGui::TableNextColumn();
        ImGui::Button("reset");

        EndTable();
      }
    }

    /* Delete object */
    if (ImGui::CollapsingHeader("Advanced"))
    {
      if (ImGui::Button("Delete object"))
      {
        /* Release resources from GPU */
        if (auto meshComp = selected.GetComponent<StaticMeshComponent>())
          meshComp->DestroyMesh();

        scene.DestroyObject(selected);
        selected.Invalidate();
      }
    }
  }
  ImGui::End();
}

/* -----------------------------------------------------
 *          PRIVATE METHODS
 * -----------------------------------------------------
*/

template<int cols>
void DetailsPanel::CreateTable(const char* label, Array<float, cols> sizes)
{
  ImGui::BeginTable(label, cols);
  for (int i = 0; i < cols; i++)
  {
    char label[8]{};
    sprintf_s(label, "##C%d", i);
    ImGui::TableSetupColumn(label, ImGuiTableColumnFlags_WidthFixed, sizes[i]);
  }
}

void DetailsPanel::EndTable()
{
  ImGui::EndTable();
}

void DetailsPanel::EditVec3(const char* label, float speed, Vec2f minMax, Vec3f& values, float colSize)
{
  ImGui::PushID(label);

  ImGui::TableNextColumn();
  ImGui::Text(label);
  ImGui::TableNextColumn();
  ImGui::SetNextItemWidth((colSize / 3.0f) - 5);
  ImGui::DragFloat("##X", &values.x, speed, minMax.x, minMax.y, "X: %.2f");
  ImGui::SameLine(0, 5);
  ImGui::SetNextItemWidth((colSize / 3.0f) - 5);
  ImGui::DragFloat("##Y", &values.y, speed, minMax.x, minMax.y, "Y: %.2f");
  ImGui::SameLine(0, 5);
  ImGui::SetNextItemWidth((colSize / 3.0f) - 5);
  ImGui::DragFloat("##Z", &values.z, speed, minMax.x, minMax.y, "Z: %.2f");

  ImGui::PopID();
}

void DetailsPanel::LightProperties(LightComponent* light)
{
  ImGui::TableNextRow();
  ImGui::TableNextColumn();
  ImGui::Text("Light color");
  ImGui::TableNextColumn();
  ImGui::SetNextItemWidth(-1);
  ImGui::ColorEdit3("##color", reinterpret_cast<float*>(&light->color));

  ImGui::TableNextRow();
  ImGui::TableNextColumn();
  ImGui::Text("Ambient");
  ImGui::TableNextColumn();
  ImGui::SetNextItemWidth(-1);
  ImGui::SliderFloat("##ambient", &light->ambient, 0.0f, 1.0f);

  ImGui::TableNextRow();
  ImGui::TableNextColumn();
  ImGui::Text("Diffuse");
  ImGui::TableNextColumn();
  ImGui::SetNextItemWidth(-1);
  ImGui::SliderFloat("##diffuse", &light->diffuse, 0.0f, 1.0f);

  ImGui::TableNextRow();
  ImGui::TableNextColumn();
  ImGui::Text("Specular");
  ImGui::TableNextColumn();
  ImGui::SetNextItemWidth(-1);
  ImGui::SliderFloat("##specular", &light->specular, 0.0f, 1.0f);
}


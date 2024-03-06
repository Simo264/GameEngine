#include "DetailsPanel.hpp"
#include "Core/Log/Logger.hpp"

#include "Engine/GameObject.hpp"

#include "Engine/ECS/Components.hpp"

#include "Engine/Graphics/Texture2D.hpp"
#include "Engine/Subsystems/TextureManager.hpp"

#include <imgui/imgui.h>

/* -----------------------------------------------------
 *          PUBLIC METHODS
 * -----------------------------------------------------
*/

void DetailsPanel::RenderPanel(GameObject& object)
{
  ImGui::Begin(panelName.c_str(), &visible);
  if (object.IsValid())
  {
    const String& label = object.GetComponent<LabelComponent>()->label;
    ImGui::Text("Object label: %s", label.c_str());
    
    /* Directional light component */
    if (auto dLightComp = object.GetComponent<DirLightComponent>())
    {
      if (ImGui::CollapsingHeader("Light properties"))
      {
        const float C1 = 0.30f * ImGui::GetContentRegionAvail().x;
        const float C2 = 0.70f * ImGui::GetContentRegionAvail().x;
        CreateTable<2>("##LightProps", { C1, C2 });
        LightProperties(dLightComp);
        EndTable();
      }
      if (ImGui::CollapsingHeader("Directional light properties"))
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
    else if (auto pLightComp = object.GetComponent<PointLightComponent>())
    {
      if (ImGui::CollapsingHeader("Light properties"))
      {
        const float C1 = 0.30f * ImGui::GetContentRegionAvail().x;
        const float C2 = 0.70f * ImGui::GetContentRegionAvail().x;
        CreateTable<2>("##LighProps", { C1, C2 });
        LightProperties(pLightComp);
        EndTable();
      }

      if (ImGui::CollapsingHeader("Point light properties"))
      {
        const float C1 = 0.30f * ImGui::GetContentRegionAvail().x;
        const float C2 = 0.70f * ImGui::GetContentRegionAvail().x;
        CreateTable<2>("##PointLightProps", { C1, C2 });
        ImGui::TableNextRow();
        EditVec3("Position", 0.1f, { -FLT_MAX, +FLT_MAX }, pLightComp->position, C2);
        EndTable();
      }
    }

    /* Transformation component */
    else if (auto transComp = object.GetComponent<TransformComponent>())
    {
      if (ImGui::CollapsingHeader("Transformation"))
      {
        const float C1 = 0.30f * ImGui::GetContentRegionAvail().x;
        const float C2 = 0.70f * ImGui::GetContentRegionAvail().x;
        CreateTable<2>("##Transformation", { C1, C2 });
        
        ImGui::TableNextRow();
        EditVec3("Position", 1, { -FLT_MAX, +FLT_MAX }, transComp->position, C2);
        ImGui::TableNextRow();
        EditVec3("Scale", 1, { -FLT_MAX, +FLT_MAX }, transComp->scale, C2);
        ImGui::TableNextRow();
        EditVec3("Rotate", 1, { -180.0f, 180.0f }, transComp->rotation, C2);
        
        EndTable();
      }
    }
    
    /* Static mesh component */
    if (auto meshComp = object.GetComponent<StaticMeshComponent>())
    {
      if (ImGui::CollapsingHeader("Material"))
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
        ImGui::Image((ImTextureID)diffuseID, { 64,64 });

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
      if (ImGui::Button("Delete object"))
        object.Destroy();
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
  ImGui::DragFloat("##X", &values.x, speed, minMax[0], minMax[1], "X: %.2f");
  ImGui::SameLine(0, 5);
  ImGui::SetNextItemWidth((colSize / 3.0f) - 5);
  ImGui::DragFloat("##Y", &values.y, speed, minMax[0], minMax[1], "Y: %.2f");
  ImGui::SameLine(0, 5);
  ImGui::SetNextItemWidth((colSize / 3.0f) - 5);
  ImGui::DragFloat("##Z", &values.z, speed, minMax[0], minMax[1], "Z: %.2f");

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


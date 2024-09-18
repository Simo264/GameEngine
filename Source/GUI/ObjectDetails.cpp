#include "Core/Core.hpp"

#include "Engine/Scene.hpp"
#include "Engine/ECS/ECS.hpp"
#include "Engine/Subsystems/TextureManager.hpp"

#include <imgui/imgui.h>
#include <imgui/ImGuizmo.h>

/* ------------------------------------------ */
/*                  PRIVATE                   */
/* ------------------------------------------ */

static constexpr const Attenuation ATTENUATION_ARRAY[] = {
  Attenuation{ 7, 0.7f, 1.8f },
  Attenuation{ 13, 0.35f, 0.44f },
  Attenuation{ 20, 0.22f, 0.20f },
  Attenuation{ 32, 0.14f, 0.07f },
  Attenuation{ 50, 0.09f, 0.032f },
  Attenuation{ 65, 0.07f, 0.017f },
  Attenuation{ 100, 0.045f, 0.0075f },
  Attenuation{ 160, 0.027f, 0.0028f },
  Attenuation{ 200, 0.022f, 0.0019f },
  Attenuation{ 325, 0.014f, 0.0007f },
  Attenuation{ 600, 0.007f, 0.0002f },
  Attenuation{ 3250, 0.0014f, 0.000007f },
};

static void DetailsModel_ComboTextures(Texture2D*& matTexture, StringView comboId, const Texture2D& defaultTex)
{
  const bool isEmpty = (matTexture->strPath.at(0) == '#');
  auto& textManager = TextureManager::Get();

  f32 panelWidth = ImGui::GetContentRegionAvail().x;
  ImGui::SetNextItemWidth(panelWidth - 32);
  if (ImGui::BeginCombo(comboId.data(), (isEmpty ? "Select texture" : matTexture->strPath.c_str())))
  {
    for (const auto& [key, texture] : textManager.GetTextures())
    {
      if (key.at(0) != '#' && ImGui::Selectable(key.c_str(), texture.Compare(*matTexture)))
        matTexture = const_cast<Texture2D*>(&texture);
    }
    ImGui::EndCombo();
  }

  if (!isEmpty)
  {
    ImGui::SameLine();

    char buttonID[32]{};
    std::format_to_n(buttonID, sizeof(buttonID), "Reset{}", comboId.data());

    static const auto& resetIcon = textManager.GetIconByPath(GetIconsPath() / "reset-arrow.png");
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.f,0.f,0.f,0.f });
    if (ImGui::ImageButton(buttonID, reinterpret_cast<void*>(resetIcon.id), { 16,16 }))
      matTexture = const_cast<Texture2D*>(&defaultTex);
    ImGui::PopStyleColor();
  }
}

static void Details_DirectLight(Entity& object, DirectionalLight& light)
{
  ImGui::BeginDisabled();
  if (ImGui::BeginCombo("Type", "Directional"))
    ImGui::EndCombo();
  ImGui::EndDisabled();

  ImGui::ColorEdit3("Color", (f32*)&light.color);
  ImGui::SliderFloat("Diffuse intensity", &light.diffuseIntensity, 0.0f, 1.0f);
  ImGui::SliderFloat("Specular intensity", &light.specularIntensity, 0.0f, 1.0f);
  ImGui::DragFloat3("Direction", (f32*)&light.direction, 0.1f, -FLT_MAX, FLT_MAX);

  ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.55f, 0.f, 0.f, 0.5f));
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.45f, 0.f, 0.f, 0.5f));
  ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.45f, 0.f, 0.f, 0.5f));
  if (ImGui::Button("Remove component##dir_light"))
  {
    object.RemoveComponent<Light>();
    object.RemoveComponent<DirectionalLight>();
  }
  ImGui::PopStyleColor(3);
}
static void Details_PointLight(Entity& object, PointLight& light)
{
  ImGui::BeginDisabled();
  if (ImGui::BeginCombo("Type", "Point"))
    ImGui::EndCombo();
  ImGui::EndDisabled();

  ImGui::ColorEdit3("Color", (f32*)&light.color);
  ImGui::SliderFloat("Diffuse intensity", &light.diffuseIntensity, 0.0f, 1.0f);
  ImGui::SliderFloat("Specular intensity", &light.specularIntensity, 0.0f, 1.0f);
  ImGui::DragFloat3("Position", (f32*)&light.position, 0.1f, -FLT_MAX, FLT_MAX);

  char currentAttRange[16]{};
  std::format_to_n(currentAttRange, sizeof(currentAttRange), "{}m", light.attenuation.range);
  if (ImGui::BeginCombo("Attenuation", currentAttRange))
  {
    char label[16]{};
    for (i32 i = 0; i < std::size(ATTENUATION_ARRAY); i++)
    {
      const auto& attenuation = ATTENUATION_ARRAY[i];
      std::fill_n(label, sizeof(label), 0);
      std::format_to_n(label, sizeof(label), "{}m", attenuation.range);
      if (ImGui::Selectable(label, light.attenuation.range == attenuation.range))
        light.attenuation = attenuation;
    }
    ImGui::EndCombo();
  }

  ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.55f, 0.f, 0.f, 0.5f));
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.45f, 0.f, 0.f, 0.5f));
  ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.45f, 0.f, 0.f, 0.5f));
  if (ImGui::Button("Remove component##point_light"))
  {
    object.RemoveComponent<Light>();
    object.RemoveComponent<PointLight>();
  }
  ImGui::PopStyleColor(3);
}
static void Details_SpotLight(Entity& object, SpotLight& light)
{
  ImGui::BeginDisabled();
  if (ImGui::BeginCombo("Type", "Spot"))
    ImGui::EndCombo();
  ImGui::EndDisabled();

  ImGui::ColorEdit3("Color", (f32*)&light.color);
  ImGui::SliderFloat("Diffuse intensity", &light.diffuseIntensity, 0.0f, 1.0f);
  ImGui::SliderFloat("Specular intensity", &light.specularIntensity, 0.0f, 1.0f);
  ImGui::DragFloat3("Direction", (f32*)&light.direction, 0.1f, -FLT_MAX, FLT_MAX);
  ImGui::DragFloat3("Position", (f32*)&light.position, 0.1f, -FLT_MAX, FLT_MAX);

  ImGui::Text("Radius");
  ImGui::Separator();
  ImGui::SliderFloat("Inner cutoff", &light.cutOff, 1.0f, light.outerCutOff);
  ImGui::SliderFloat("Outer cutoff", &light.outerCutOff, light.cutOff, 45.0f);

  char currentAttRange[16]{};
  std::format_to_n(currentAttRange, sizeof(currentAttRange), "{}m", light.attenuation.range);
  if (ImGui::BeginCombo("Attenuation", currentAttRange))
  {
    char label[16]{};
    for (i32 i = 0; i < std::size(ATTENUATION_ARRAY); i++)
    {
      const auto& attenuation = ATTENUATION_ARRAY[i];
      std::fill_n(label, sizeof(label), 0);
      std::format_to_n(label, sizeof(label), "{}m", attenuation.range);
      if (ImGui::Selectable(label, light.attenuation.range == attenuation.range))
        light.attenuation = attenuation;
    }
    ImGui::EndCombo();
  }


  ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.55f, 0.f, 0.f, 0.5f));
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.45f, 0.f, 0.f, 0.5f));
  ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.45f, 0.f, 0.f, 0.5f));
  if (ImGui::Button("Remove component##spot_light"))
  {
    object.RemoveComponent<Light>();
    object.RemoveComponent<SpotLight>();
  }
  ImGui::PopStyleColor(3);
}
static void Details_Transform(Entity& object, Transform& transform, i32& gizmode)
{
  ImGui::RadioButton("Translate", &gizmode, ImGuizmo::OPERATION::TRANSLATE);
  ImGui::RadioButton("Rotate", &gizmode, ImGuizmo::OPERATION::ROTATE);
  ImGui::RadioButton("Scale", &gizmode, ImGuizmo::OPERATION::SCALE);

  ImGui::DragFloat3("Position", (f32*)&transform.position, 0.1f, -FLT_MAX, FLT_MAX);
  ImGui::DragFloat3("Scale", (f32*)&transform.scale, 0.1f, -FLT_MAX, FLT_MAX);
  ImGui::DragFloat3("Rotation", (f32*)&transform.rotation, 0.1f, -FLT_MAX, FLT_MAX);
  transform.UpdateTransformation();

  ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.55f, 0.f, 0.f, 0.5f));
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.45f, 0.f, 0.f, 0.5f));
  ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.45f, 0.f, 0.f, 0.5f));
  if (ImGui::Button("Remove component##transform"))
    object.RemoveComponent<Transform>();
  ImGui::PopStyleColor(3);
}
static void Details_Model(Entity& object, Model& model)
{
  /* Display model path */
  ImGui::TextWrapped("Model: %s", model.strPath.c_str());
  ImGui::Separator();

  /* Display the number of meshes */
  const i64 numMeshes = model.meshes.size();
  ImGui::TextWrapped("Meshes: %d", numMeshes);

  /* For each mesh */
  for (i32 i = 0; i < numMeshes; i++)
  {
    auto& mesh = model.meshes.at(i);
    Material& material = mesh.material;
    TextureManager& texManager = TextureManager::Get();

    /* Display mesh-i props */
    char title[16]{};
    std::format_to_n(title, sizeof(title), "Mesh {}", i);
    ImGui::SeparatorText(title);

    ImGui::Text("Diffuse");
    DetailsModel_ComboTextures(material.diffuse, "##Diffuse", texManager.GetDefaultDiffuse());
    ImGui::InvisibleButton("##margin_bottom", ImVec2(1.f, 5.0f));

    ImGui::Text("Specular");
    DetailsModel_ComboTextures(material.specular, "##Specular", texManager.GetDefaultSpecular());
    ImGui::InvisibleButton("##margin_bottom", ImVec2(1.f, 5.0f));

    ImGui::Text("Normal");
    DetailsModel_ComboTextures(material.normal, "##Normal", texManager.GetDefaultNormal());
    ImGui::InvisibleButton("##margin_bottom", ImVec2(1.f, 5.0f));

    ImGui::Text("Height");
    DetailsModel_ComboTextures(material.height, "##Height", texManager.GetDefaultHeight());
  }

  ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.55f, 0.f, 0.f, 0.5f));
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.45f, 0.f, 0.f, 0.5f));
  ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.45f, 0.f, 0.f, 0.5f));
  if (ImGui::Button("Remove component##model"))
    object.RemoveComponent<Model>();
  ImGui::PopStyleColor(3);
}

/* ------------------------------------------ */
/*                    PUBLIC                  */
/* ------------------------------------------ */

void GUI_RenderObjectDetails(bool& open, Entity& object, i32& gizmode)
{
  ImGui::Begin("Details", &open);
  if (object.IsValid())
  {
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.f, 0.85f, 0.f, 1.f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
    if (ImGui::Button("+Add component"))
    {
      CONSOLE_DEBUG("TODO");
    }
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();

    if (auto* transform = object.GetComponent<Transform>())
    {
      if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
        Details_Transform(object, *transform, gizmode);
    }
    if (auto* model = object.GetComponent<Model>())
    {
      if (ImGui::CollapsingHeader("Model", ImGuiTreeNodeFlags_DefaultOpen))
        Details_Model(object, *model);
    }
    if (auto* light = object.GetComponent<Light>())
    {
      if (ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_DefaultOpen))
      {
        switch (light->type)
        {
        case LightType::DIRECTIONAL:
        {
          auto* dirLight = object.GetComponent<DirectionalLight>();
          Details_DirectLight(object, *dirLight);
          break;
        }
        case LightType::POINT:
        {
          auto* pointLight = object.GetComponent<PointLight>();
          Details_PointLight(object, *pointLight);
          break;
        }
        case LightType::SPOT:
        {
          auto* spotLight = object.GetComponent<SpotLight>();
          Details_SpotLight(object, *spotLight);
          break;
        }
        }
      }
    }
  }
  ImGui::End();
}

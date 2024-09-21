#include "Core/Core.hpp"

#include "Engine/Scene.hpp"
#include "Engine/ECS/ECS.hpp"
#include "Engine/Subsystems/TextureManager.hpp"
#include "Engine/Subsystems/ShaderManager.hpp"
#include "Engine/Filesystem/Dialog.hpp"

#include <imgui/imgui.h>
#include <imgui/ImGuizmo.h>


/* ------------------------------------------ */
/*                  PRIVATE                   */
/* ------------------------------------------ */

static bool ButtonCentered(const char* label, ImVec2 size)
{
  f32 avail = ImGui::GetContentRegionAvail().x;
  f32 off = (avail - size.x) * 0.5f;
  if (off > 0.0f)
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);

  return ImGui::Button(label, size);
}

static void Insp_Light_ComboLightType(StringView preview, bool dir, bool point, bool spot)
{
  if (ImGui::BeginCombo("Type", preview.data(), ImGuiComboFlags_NoArrowButton))
  {
    ImGui::Selectable("Directional", dir, !dir ? ImGuiSelectableFlags_Disabled : 0);
    ImGui::Selectable("Point", point, !point ? ImGuiSelectableFlags_Disabled : 0);
    ImGui::Selectable("Spot", spot, !spot ? ImGuiSelectableFlags_Disabled : 0);
    ImGui::EndCombo();
  }
}
static void Insp_DirectLight(GameObject& object, DirectionalLight& light)
{
  Insp_Light_ComboLightType("Directional", true, false, false);

  ImGui::ColorEdit3("Color", (f32*)&light.color);
  ImGui::SliderFloat("Diffuse intensity", &light.diffuseIntensity, 0.0f, 1.0f);
  ImGui::SliderFloat("Specular intensity", &light.specularIntensity, 0.0f, 1.0f);
  ImGui::DragFloat3("Direction", (f32*)&light.direction, 0.1f, -FLT_MAX, FLT_MAX);

  ImGui::SeparatorText("Advanced");
  ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.55f, 0.f, 0.f, 0.5f));
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.45f, 0.f, 0.f, 0.5f));
  ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.45f, 0.f, 0.f, 0.5f));
  if (ImGui::Button("Remove component##dir_light"))
  {
    object.RemoveComponent<Light>();
    object.RemoveComponent<DirectionalLight>();

    auto& shaderManager = ShaderManager::Get();
    shaderManager.GetProgramByName("Scene").Link();
    shaderManager.GetProgramByName("SceneShadows").Link();
    shaderManager.ResetProgramsUniforms();
  }
  ImGui::PopStyleColor(3);
}
static void Insp_PointLight(GameObject& object, PointLight& light)
{
  Insp_Light_ComboLightType("Point", false, true, false);

  ImGui::ColorEdit3("Color", (f32*)&light.color);
  ImGui::SliderFloat("Diffuse intensity", &light.diffuseIntensity, 0.0f, 1.0f);
  ImGui::SliderFloat("Specular intensity", &light.specularIntensity, 0.0f, 1.0f);
  ImGui::DragFloat3("Position", (f32*)&light.position, 0.1f, -FLT_MAX, FLT_MAX);

  ImGui::SeparatorText("Attenuation");
  
  char currentAttRange[16]{};
  std::format_to_n(currentAttRange, sizeof(currentAttRange), "{}m", light.attenuation.range);
  if (ImGui::BeginCombo("Range", currentAttRange))
  {
    char label[16]{};
    for (i32 i = 0; i < std::size(ATTENUATION_RANGES); i++)
    {
      const auto& attenuation = ATTENUATION_RANGES[i];
      std::fill_n(label, sizeof(label), 0);
      std::format_to_n(label, sizeof(label), "{}m", attenuation.range);
      if (ImGui::Selectable(label, light.attenuation.range == attenuation.range))
        light.attenuation = attenuation;
    }
    ImGui::EndCombo();
  }

  ImGui::SeparatorText("Advanced");
  ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.55f, 0.f, 0.f, 0.5f));
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.45f, 0.f, 0.f, 0.5f));
  ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.45f, 0.f, 0.f, 0.5f));
  if (ImGui::Button("Remove component##point_light"))
  {
    object.RemoveComponent<Light>();
    object.RemoveComponent<PointLight>();

    auto& shaderManager = ShaderManager::Get();
    shaderManager.GetProgramByName("Scene").Link();
    shaderManager.GetProgramByName("SceneShadows").Link();
    shaderManager.ResetProgramsUniforms();
  }
  ImGui::PopStyleColor(3);
}
static void Insp_SpotLight(GameObject& object, SpotLight& light)
{
  Insp_Light_ComboLightType("Spot", false, false, true);

  ImGui::ColorEdit3("Color", (f32*)&light.color);
  ImGui::SliderFloat("Diffuse intensity", &light.diffuseIntensity, 0.0f, 1.0f);
  ImGui::SliderFloat("Specular intensity", &light.specularIntensity, 0.0f, 1.0f);
  ImGui::DragFloat3("Direction", (f32*)&light.direction, 0.1f, -FLT_MAX, FLT_MAX);
  ImGui::DragFloat3("Position", (f32*)&light.position, 0.1f, -FLT_MAX, FLT_MAX);

  ImGui::SeparatorText("Radius");
  
  ImGui::SliderFloat("Inner cutoff", &light.cutOff, 1.0f, light.outerCutOff);
  ImGui::SliderFloat("Outer cutoff", &light.outerCutOff, light.cutOff, 45.0f);

  ImGui::SeparatorText("Attenuation");
  
  char currentAttRange[16]{};
  std::format_to_n(currentAttRange, sizeof(currentAttRange), "{}m", light.attenuation.range);
  if (ImGui::BeginCombo("Range", currentAttRange))
  {
    char label[16]{};
    for (i32 i = 0; i < std::size(ATTENUATION_RANGES); i++)
    {
      const auto& attenuation = ATTENUATION_RANGES[i];
      std::fill_n(label, sizeof(label), 0);
      std::format_to_n(label, sizeof(label), "{}m", attenuation.range);
      if (ImGui::Selectable(label, light.attenuation.range == attenuation.range))
        light.attenuation = attenuation;
    }
    ImGui::EndCombo();
  }

  ImGui::SeparatorText("Advanced");
  ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.55f, 0.f, 0.f, 0.5f));
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.45f, 0.f, 0.f, 0.5f));
  ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.45f, 0.f, 0.f, 0.5f));
  if (ImGui::Button("Remove component##spot_light"))
  {
    object.RemoveComponent<Light>();
    object.RemoveComponent<SpotLight>();

    auto& shaderManager = ShaderManager::Get();
    shaderManager.GetProgramByName("Scene").Link();
    shaderManager.GetProgramByName("SceneShadows").Link();
    shaderManager.ResetProgramsUniforms();
  }
  ImGui::PopStyleColor(3);
}
static void Insp_Transform(GameObject& object, Transform& transform, i32& gizmode)
{
  ImGui::RadioButton("Translate", &gizmode, ImGuizmo::OPERATION::TRANSLATE);
  ImGui::RadioButton("Rotate", &gizmode, ImGuizmo::OPERATION::ROTATE);
  ImGui::RadioButton("Scale", &gizmode, ImGuizmo::OPERATION::SCALE);

  ImGui::DragFloat3("Position", (f32*)&transform.position, 0.1f, -FLT_MAX, FLT_MAX);
  ImGui::DragFloat3("Scale", (f32*)&transform.scale, 0.1f, -FLT_MAX, FLT_MAX);
  ImGui::DragFloat3("Rotation", (f32*)&transform.rotation, 0.1f, -FLT_MAX, FLT_MAX);
  transform.UpdateTransformation();

  ImGui::SeparatorText("Advanced");
  ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.55f, 0.f, 0.f, 0.5f));
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.45f, 0.f, 0.f, 0.5f));
  ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.45f, 0.f, 0.f, 0.5f));
  if (ImGui::Button("Remove component##transform"))
    object.RemoveComponent<Transform>();
  ImGui::PopStyleColor(3);
}
static void Insp_Model_ComboTextures(Texture2D*& matTexture, StringView comboId, const Texture2D& defaultTex)
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

    static const auto& resetIcon = textManager.GetIconByPath(GetIconsPath() / "reset-arrow-16.png");
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.f,0.f,0.f,0.f });
    if (ImGui::ImageButton(buttonID, reinterpret_cast<void*>(resetIcon.id), ImVec2(16.f, 16.f)))
      matTexture = const_cast<Texture2D*>(&defaultTex);
    ImGui::PopStyleColor();
  }
}
static void Insp_Model(GameObject& object, Model& model)
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
    Insp_Model_ComboTextures(material.diffuse, "##Diffuse", texManager.GetDefaultDiffuse());
    ImGui::InvisibleButton("##margin_bottom", ImVec2(1.f, 5.0f));

    ImGui::Text("Specular");
    Insp_Model_ComboTextures(material.specular, "##Specular", texManager.GetDefaultSpecular());
    ImGui::InvisibleButton("##margin_bottom", ImVec2(1.f, 5.0f));

    ImGui::Text("Normal");
    Insp_Model_ComboTextures(material.normal, "##Normal", texManager.GetDefaultNormal());
    ImGui::InvisibleButton("##margin_bottom", ImVec2(1.f, 5.0f));

    ImGui::Text("Height");
    Insp_Model_ComboTextures(material.height, "##Height", texManager.GetDefaultHeight());
  }

  ImGui::SeparatorText("Advanced");
  ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.55f, 0.f, 0.f, 0.5f));
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.45f, 0.f, 0.f, 0.5f));
  ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.45f, 0.f, 0.f, 0.5f));
  if (ImGui::Button("Remove component##model"))
    object.RemoveComponent<Model>();
  ImGui::PopStyleColor(3);
}

static void Insp_ListAllComponents(GameObject& object, i32& gizmode)
{
  if (ImGui::CollapsingHeader("Tag"))
  {
    auto* tag = object.GetComponent<Tag>();
    ImGui::InputText("Value", tag->value, sizeof(tag->value), ImGuiInputTextFlags_CharsNoBlank);
  }

  if (auto* transform = object.GetComponent<Transform>())
  {
    if (ImGui::CollapsingHeader("Transform"))
      Insp_Transform(object, *transform, gizmode);
  }
  if (auto* model = object.GetComponent<Model>())
  {
    if (ImGui::CollapsingHeader("Model"))
      Insp_Model(object, *model);
  }
  if (auto* light = object.GetComponent<Light>())
  {
    if (ImGui::CollapsingHeader("Light"))
    {
      switch (light->type)
      {
      case LightType::DIRECTIONAL:
      {
        auto* dirLight = object.GetComponent<DirectionalLight>();
        Insp_DirectLight(object, *dirLight);
        break;
      }
      case LightType::POINT:
      {
        auto* pointLight = object.GetComponent<PointLight>();
        Insp_PointLight(object, *pointLight);
        break;
      }
      case LightType::SPOT:
      {
        auto* spotLight = object.GetComponent<SpotLight>();
        Insp_SpotLight(object, *spotLight);
        break;
      }
      }
    }
  }
}
static void Insp_NewComponentPopup(GameObject& object)
{
  if (ImGui::BeginPopup("NewComponent_Popup"))
  {
    if (object.HasComponent<Transform>()) /* Component already present, disabled */
      ImGui::Selectable("Transform", false, ImGuiSelectableFlags_Disabled);
    else if (ImGui::Selectable("Transform"))
      object.AddComponent<Transform>();

    ImGui::Spacing();

    if (object.HasComponent<Model>()) /* Component already present, disabled */
      ImGui::Selectable("Model", false, ImGuiSelectableFlags_Disabled);
    else if (ImGui::BeginMenu("Model"))
    {
      ImGui::BeginChild("Model_Child", ImVec2(300, 100));
      static String path{};
      if (ImGui::Button("Open model"))
      {
        const char* filter[] = { "*.obj" };
        path = OpenFileDialog(1, filter, "Object file", false);
      }
      ImGui::SameLine();
      ImGui::InputText("##", path.data(), path.size(), ImGuiInputTextFlags_ReadOnly);

      if (!path.empty())
      {
        if (ImGui::Button("Ok"))
        {
          object.AddComponent<Model>(path);
          path.clear();
        }
      }
      else
      {
        ImGui::BeginDisabled();
        ImGui::Button("Ok");
        ImGui::EndDisabled();
      }
      ImGui::EndChild();
      ImGui::EndMenu();
    }

    ImGui::Spacing();

    if (object.HasComponent<Light>()) /* Component already present, disabled */
      ImGui::Selectable("Light", false, ImGuiSelectableFlags_Disabled);
    else if (ImGui::BeginMenu("Light"))
    {
      if (ImGui::MenuItem("Directional"))
      {
        object.AddComponent<Light>(LightType::DIRECTIONAL);
        object.AddComponent<DirectionalLight>();
      }
      if (ImGui::MenuItem("Point"))
      {
        object.AddComponent<Light>(LightType::POINT);
        object.AddComponent<PointLight>();
      }
      if (ImGui::MenuItem("Spot"))
      {
        object.AddComponent<Light>(LightType::SPOT);
        object.AddComponent<SpotLight>();
      }
      ImGui::EndMenu();
    }

    ImGui::EndPopup();
  }
}



/* ------------------------------------------ */
/*                    PUBLIC                  */
/* ------------------------------------------ */


void GUI_RenderInspector(bool& open, GameObject& object, i32& gizmode)
{
  ImGui::Begin("Details", &open);
  if (object.IsValid())
  {
    /* "+New component" button */
    f32 btnWidth = ImGui::GetContentRegionAvail().x - 32.f;
    if (ButtonCentered("+Add component", ImVec2(btnWidth, 26.f)))
      ImGui::OpenPopup("NewComponent_Popup");

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    /* List all components */
    Insp_ListAllComponents(object, gizmode);
  }

  /* Open popup on click to "+New component" button */
  Insp_NewComponentPopup(object);

  ImGui::End();
}

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
static void DetailsLight_ComboLightType(StringView preview, bool dir, bool point, bool spot)
{
  if (ImGui::BeginCombo("Type", preview.data(), ImGuiComboFlags_NoArrowButton))
  {
    ImGui::Selectable("Directional", dir, !dir ? ImGuiSelectableFlags_Disabled : 0);
    ImGui::Selectable("Point", point, !point ? ImGuiSelectableFlags_Disabled : 0);
    ImGui::Selectable("Spot", spot, !spot ? ImGuiSelectableFlags_Disabled : 0);
    ImGui::EndCombo();
  }
}

static void Details_DirectLight(Entity& object, DirectionalLight& light)
{
  DetailsLight_ComboLightType("Directional", true, false, false);

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
static void Details_PointLight(Entity& object, PointLight& light)
{
  DetailsLight_ComboLightType("Point", false, true, false);

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
static void Details_SpotLight(Entity& object, SpotLight& light)
{
  DetailsLight_ComboLightType("Spot", false, false, true);

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
static void Details_Transform(Entity& object, Transform& transform, i32& gizmode)
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

  ImGui::SeparatorText("Advanced");
  ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.55f, 0.f, 0.f, 0.5f));
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.45f, 0.f, 0.f, 0.5f));
  ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.45f, 0.f, 0.f, 0.5f));
  if (ImGui::Button("Remove component##model"))
    object.RemoveComponent<Model>();
  ImGui::PopStyleColor(3);
}

static void Details_NewComponentButton()
{
  ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.f, 0.85f, 0.f, 1.f));
  ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
  
  if (ImGui::Button("+Add component"))
    ImGui::OpenPopup("EntityPopup");
  
  ImGui::PopStyleVar();
  ImGui::PopStyleColor();
}
static void Details_ListAllComponents(Entity& object, i32& gizmode)
{
  if (auto* transform = object.GetComponent<Transform>())
  {
    if (ImGui::CollapsingHeader("Transform"))
      Details_Transform(object, *transform, gizmode);
  }
  if (auto* model = object.GetComponent<Model>())
  {
    if (ImGui::CollapsingHeader("Model"))
      Details_Model(object, *model);
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

static bool openTransformWindow = false;
static bool openModelWindow = false;
static bool openLightWindow = false;
static void NewComponentPopup_Selectable(StringView label)
{
  ImGui::BeginDisabled();
  ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.f, 0.f, 0.f, 0.f));
  ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.f, 0.f, 0.f, 0.f));
  ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.f, 0.f, 0.f, 0.f));
  ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.f, 0.f));
  bool checked = true;
  ImGui::Checkbox("##", &checked);
  ImGui::PopStyleVar();
  ImGui::PopStyleColor(3);
  ImGui::SameLine();
  ImGui::Selectable(label.data());
  ImGui::EndDisabled();
}
static void Details_NewComponentPopup(Entity& object)
{
  if (ImGui::BeginPopup("EntityPopup"))
  {
    /* List of all components */
    if (object.HasComponent<Transform>()) /* Component already present, disabled */
      NewComponentPopup_Selectable("Transform");
    else if (ImGui::Selectable("Transform"))
      openTransformWindow = true;

    ImGui::Spacing();

    if (object.HasComponent<Model>()) /* Component already present, disabled */
      NewComponentPopup_Selectable("Model");
    else if (ImGui::Selectable("Model"))
      openModelWindow = true;

    ImGui::Spacing();

    if (object.HasComponent<Light>()) /* Component already present, disabled */
      NewComponentPopup_Selectable("Light");
    else if (ImGui::Selectable("Light"))
      openLightWindow = true;

    ImGui::EndPopup();
  }
}


/* ------------------------------------------ */
/*                    PUBLIC                  */
/* ------------------------------------------ */

void GUI_RenderObjectDetails(bool& open, Entity& object, i32& gizmode)
{
  ImGui::Begin("Details", &open);
  if (object.IsValid())
  {
    /* "+New component" button */
    Details_NewComponentButton();

    /* List all components */
    Details_ListAllComponents(object, gizmode);
  }

  /* Open popup on click "+New component" button */
  Details_NewComponentPopup(object);
  
  if (openTransformWindow)
  {
    static Transform tmp_transform{};
    ImVec2 pos = ImGui::GetMainViewport()->GetCenter();
    pos.x -= 250.f;
    pos.y -= 150.f;
    ImGui::SetNextWindowPos(pos, ImGuiCond_Appearing);
    ImGui::SetNextWindowSize(ImVec2(500.f, 300.f), ImGuiCond_Appearing);
    ImGui::Begin("Transform component", &openTransformWindow, ImGuiWindowFlags_NoDocking);
    
    ImGui::InputFloat3("Position", reinterpret_cast<f32*>(&tmp_transform.position));
    ImGui::InputFloat3("Scale", reinterpret_cast<f32*>(&tmp_transform.scale));
    ImGui::InputFloat3("Rotation", reinterpret_cast<f32*>(&tmp_transform.rotation));
    if (ImGui::Button("Done"))
    {
      auto& transform = object.AddComponent<Transform>();
      transform = tmp_transform;
      openTransformWindow = false;
      tmp_transform = Transform{};
    }
    ImGui::SameLine();
    if (ImGui::Button("Cancel"))
    {
      openTransformWindow = false;
      tmp_transform = Transform{};
    }
    ImGui::End();
  }
  else if (openModelWindow)
  {
    static String tmp_modelPath{};
    ImVec2 pos = ImGui::GetMainViewport()->GetCenter();
    pos.x -= 250.f;
    pos.y -= 150.f;
    ImGui::SetNextWindowPos(pos, ImGuiCond_Appearing);
    ImGui::SetNextWindowSize(ImVec2(500.f, 300.f), ImGuiCond_Appearing);
    ImGui::Begin("Model component", &openModelWindow, ImGuiWindowFlags_NoDocking);
    if (ImGui::Button("Select model file"))
    {
      const char* filter[] = { "*.obj" };
      String path = OpenFileDialog(1, filter, "Select .obj file", false);
      if (!path.empty())
        tmp_modelPath = path;
    }
    ImGui::SameLine();
    ImGui::InputText("##model_path", tmp_modelPath.data(), tmp_modelPath.size(), ImGuiInputTextFlags_ReadOnly);

    if (tmp_modelPath.empty())
    {
      ImGui::BeginDisabled();
      ImGui::Button("Done");
      ImGui::EndDisabled();
    }
    else if(ImGui::Button("Done"))
    {
      object.AddComponent<Model>(tmp_modelPath);
      openModelWindow = false;
      tmp_modelPath = String{};
    }
    ImGui::SameLine();
    if (ImGui::Button("Cancel"))
    {
      openModelWindow = false;
      tmp_modelPath = String{};
    }

    ImGui::End();
  }
  else if (openLightWindow)
  {
    static constexpr const char* typeOfLights[] = {
      "Directional",
      "Point",
      "Spot"
    };
    static i32 tmp_lightIdx = 0;
    static DirectionalLight tmp_dirLight{};
    static PointLight tmp_pointLight{};
    static SpotLight tmp_spotLight{};

    ImVec2 pos = ImGui::GetMainViewport()->GetCenter();
    pos.x -= 250.f;
    pos.y -= 150.f;
    ImGui::SetNextWindowPos(pos, ImGuiCond_Appearing);
    ImGui::SetNextWindowSize(ImVec2(500.f, 300.f), ImGuiCond_Appearing);
    ImGui::Begin("Light component", &openLightWindow, ImGuiWindowFlags_NoDocking);

    if (ImGui::BeginCombo("Type", typeOfLights[tmp_lightIdx]))
    {
      for (i32 i = 0; i < std::size(typeOfLights); i++)
        if (ImGui::Selectable(typeOfLights[i], tmp_lightIdx == i))
          tmp_lightIdx = i;

      ImGui::EndCombo();
    }
    switch (tmp_lightIdx)
    {
    case 0: /* Directional light */
    {
      ImGui::InputFloat3("Color", reinterpret_cast<f32*>(&tmp_dirLight.color));
      ImGui::InputFloat("Diffuse intensity", reinterpret_cast<f32*>(&tmp_dirLight.diffuseIntensity));
      ImGui::InputFloat("Specular intensity", reinterpret_cast<f32*>(&tmp_dirLight.specularIntensity));
      ImGui::InputFloat3("Direction", reinterpret_cast<f32*>(&tmp_dirLight.direction));
      break;
    }
    case 1: /* Point light */
    {
      ImGui::InputFloat3("Color", reinterpret_cast<f32*>(&tmp_pointLight.color));
      ImGui::InputFloat("Diffuse intensity", reinterpret_cast<f32*>(&tmp_pointLight.diffuseIntensity));
      ImGui::InputFloat("Specular intensity", reinterpret_cast<f32*>(&tmp_pointLight.specularIntensity));
      ImGui::InputFloat3("Position", reinterpret_cast<f32*>(&tmp_pointLight.position));
      char currentAttRange[16]{};
      std::format_to_n(currentAttRange, sizeof(currentAttRange), "{}m", tmp_pointLight.attenuation.range);
      if (ImGui::BeginCombo("Attenuation range", currentAttRange))
      {
        char label[16]{};
        for (i32 i = 0; i < std::size(ATTENUATION_RANGES); i++)
        {
          const auto& attenuation = ATTENUATION_RANGES[i];
          std::fill_n(label, sizeof(label), 0);
          std::format_to_n(label, sizeof(label), "{}m", attenuation.range);
          if (ImGui::Selectable(label, tmp_pointLight.attenuation.range == attenuation.range))
            tmp_pointLight.attenuation = attenuation;
        }
        ImGui::EndCombo();
      }
      break;
    }
    case 2: /* Spot light */
    {
      ImGui::InputFloat3("Color", reinterpret_cast<f32*>(&tmp_spotLight.color));
      ImGui::InputFloat("Diffuse intensity", reinterpret_cast<f32*>(&tmp_spotLight.diffuseIntensity));
      ImGui::InputFloat("Specular intensity", reinterpret_cast<f32*>(&tmp_spotLight.specularIntensity));
      ImGui::InputFloat3("Direction", reinterpret_cast<f32*>(&tmp_spotLight.direction));
      ImGui::InputFloat3("Position", reinterpret_cast<f32*>(&tmp_spotLight.position));
      char currentAttRange[16]{};
      std::format_to_n(currentAttRange, sizeof(currentAttRange), "{}m", tmp_spotLight.attenuation.range);
      if (ImGui::BeginCombo("Attenuation range", currentAttRange))
      {
        char label[16]{};
        for (i32 i = 0; i < std::size(ATTENUATION_RANGES); i++)
        {
          const auto& attenuation = ATTENUATION_RANGES[i];
          std::fill_n(label, sizeof(label), 0);
          std::format_to_n(label, sizeof(label), "{}m", attenuation.range);
          if (ImGui::Selectable(label, tmp_spotLight.attenuation.range == attenuation.range))
            tmp_spotLight.attenuation = attenuation;
        }
        ImGui::EndCombo();
      }
      ImGui::InputFloat("Cutoff", reinterpret_cast<f32*>(&tmp_spotLight.cutOff));
      ImGui::InputFloat("Outer cutoff", reinterpret_cast<f32*>(&tmp_spotLight.outerCutOff));
      break;
    }
    }

    if (ImGui::Button("Done"))
    {
      switch (tmp_lightIdx)
      {
      case 0: /* Directional light */
      {
        object.AddComponent<Light>(LightType::DIRECTIONAL);
        auto& light = object.AddComponent<DirectionalLight>();
        light = tmp_dirLight;
        break;
      }
      case 1: /* Point light */
      {
        object.AddComponent<Light>(LightType::POINT);
        auto& light = object.AddComponent<PointLight>();
        light = tmp_pointLight;
        break;
      }
      case 2: /* Spot light */
      {
        object.AddComponent<Light>(LightType::SPOT);
        auto& light = object.AddComponent<SpotLight>();
        light = tmp_spotLight;
        break;
      }
      }

      openLightWindow = false;
      tmp_lightIdx = 0;
      tmp_dirLight = DirectionalLight{};
      tmp_pointLight = PointLight{};
      tmp_spotLight = SpotLight{};
    }
    ImGui::SameLine();
    if (ImGui::Button("Cancel"))
    {
      openLightWindow = false;
      tmp_lightIdx = 0;
      tmp_dirLight = DirectionalLight{};
      tmp_pointLight = PointLight{};
      tmp_spotLight = SpotLight{};
    }
    ImGui::End();
  }

  ImGui::End();
}

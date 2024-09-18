#include "Core/Core.hpp"
#include "Engine/Scene.hpp"
#include "Engine/ECS/ECS.hpp"
#include "Engine/Subsystems/ShaderManager.hpp"
#include "Engine/Filesystem/Dialog.hpp"

#include "ImGuiLayer.hpp"

#include <imgui/imgui.h>

/* -------------------------- */
/*          PRIVATE           */
/* -------------------------- */

static i32 tmp_compIdx = 0;
static i32 tmp_lightIdx = 0;
static Label tmp_label{ "Object" };
static Transform tmp_transform{};
static String tmp_modelPath{};
static DirectionalLight tmp_dirLight{};
static PointLight tmp_pointLight{};
static SpotLight tmp_spotLight{};

/**
 * First: the name of component 
 * Second: flag that the component has been marked as to be added
 */
static std::pair<const char*, bool> compToInclude[] = {
  { "Label",      true  },
  { "Transform",  false },
  { "Model",      false },
  { "Light",      false },
};
static constexpr const char* typeOfLights[] = {
  "Directional",
  "Point",
  "Spot"
};

static void ResetValues()
{
  tmp_label.UpdateValue("Object");
  tmp_transform = Transform{};
  tmp_modelPath = String{};
  tmp_dirLight = DirectionalLight{};
  tmp_pointLight = PointLight{};
  tmp_spotLight = SpotLight{};
  tmp_compIdx = 0;
  tmp_lightIdx = 0;
  for (i32 i = 1; i < std::size(compToInclude); i++)
    compToInclude[i].second = false;
}

static void Left_ComponentList()
{
  for (i32 i = 0; i < std::size(compToInclude); i++)
  {
    const auto& component = compToInclude[i].first;
    const auto& checked = compToInclude[i].second;
    if (checked)
    {
      ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.13f, 0.6f, 0.33f, 0.5f));
      ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.11f, 0.51f, 0.28f, 0.5f));
      ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.11f, 0.51f, 0.28f, 0.5f));
    }

    if (ImGui::Selectable(component, checked || tmp_compIdx == i))
      tmp_compIdx = i;

    if (checked)
      ImGui::PopStyleColor(3);

    ImGui::Spacing();
  }
}
static void Right_LabelView()
{
  ImGui::SeparatorText("Label component");
  ImGui::BeginDisabled(true);
  bool check = true;
  ImGui::Checkbox("Add component", &check);
  ImGui::EndDisabled();
  ImGui::InputText("Value", tmp_label.value, sizeof(tmp_label.value));
}
static void Right_TransformView(bool& check)
{
  ImGui::SeparatorText("Transform component");
  ImGui::Checkbox("Add component", &check);
  if (check)
  {
    ImGui::InputFloat3("Position", reinterpret_cast<f32*>(&tmp_transform.position));
    ImGui::InputFloat3("Scale", reinterpret_cast<f32*>(&tmp_transform.scale));
    ImGui::InputFloat3("Rotation", reinterpret_cast<f32*>(&tmp_transform.rotation));
  }
}
static void Right_ModelView(bool& check)
{
  ImGui::SeparatorText("Model component");
  ImGui::Checkbox("Add component", &check);
  if (check)
  {
    if (ImGui::Button("Select model file"))
    {
      const char* filter[] = { "*.obj" };
      String path = OpenFileDialog(1, filter, "Select .obj file", false);
      if (!path.empty())
        tmp_modelPath = path;
    }
    ImGui::SameLine();
    ImGui::InputText("##model_path", tmp_modelPath.data(), tmp_modelPath.size(), ImGuiInputTextFlags_ReadOnly);
  }
}
static void Right_LightView(bool& check)
{
  ImGui::SeparatorText("Light component");
  ImGui::Checkbox("Add component", &check);
  if (check)
  {
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
  }
}
static bool Right_CheckValues(const Label& label, StringView modelPath, char msg[], i32 size)
{
  if (std::strlen(label.value) == 0)
  {
    std::format_to_n(msg, size, "Invalid label value (len=0)");
    return false;
  }
  bool modelChecked = compToInclude[2].second;
  if (modelChecked && modelPath.empty())
  {
    std::format_to_n(msg, size, "Invalid model path (len=0)");
    return false;
  }
  return true;
}

static void ShowDialog(StringView name, StringView message, bool& open)
{
  /* Always center this window when appearing */
  ImVec2 center = ImGui::GetMainViewport()->GetCenter();
  ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
  ImGui::SetNextWindowSize(ImVec2(300.f, 50.f), ImGuiCond_Appearing);
  ImGui::BeginPopupModal(name.data(), nullptr, ImGuiWindowFlags_AlwaysAutoResize);
  ImGui::TextWrapped(message.data());
  ImGui::Separator();

  f32 width = ImGui::GetContentRegionAvail().x;
  if (ImGui::Button("OK", ImVec2(width, 20.f)))
  {
    open = false;
    ImGui::CloseCurrentPopup();
  }

  ImGui::SetItemDefaultFocus();
  ImGui::EndPopup();
}
static void Outliner_NewEntityButton(bool& openEntityConfig)
{
  ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.f, 0.85f, 0.f, 1.f));
  ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
  if (ImGui::Button("+New object"))
    openEntityConfig = true;
  ImGui::PopStyleVar();
  ImGui::PopStyleColor();
}
static void Outliner_ListEntities(Scene& scene, Entity& objSelected)
{
  char selectableName[64]{};
  for (auto [entity, label] : scene.Reg().view<Label>().each())
  {
    std::fill_n(selectableName, sizeof(selectableName), 0);
    std::format_to_n(selectableName, sizeof(selectableName), "{}##{}", label.value, static_cast<u32>(entity));

    Entity o{ entity, &scene.Reg() };
    bool selected = ImGui::Selectable(selectableName, objSelected.IsEqual(o));
    if (selected && !objSelected.IsEqual(o))
      objSelected = o;
    if (
      ImGui::IsItemHovered() &&
      ImGui::IsMouseClicked(ImGuiMouseButton_Right) &&
      objSelected.IsValid()
      )
      ImGui::OpenPopup("ObjectMenu");

    ImGui::Spacing();
  }
}
static void Outliner_CreateEntityWindow(bool& open, Scene& scene)
{
  ImVec2 pos = ImGui::GetMainViewport()->GetCenter();
  pos.x -= 250.f;
  pos.y -= 150.f;
  ImGui::SetNextWindowPos(pos, ImGuiCond_Appearing);
  ImGui::SetNextWindowSize(ImVec2(500.f, 300.f), ImGuiCond_Appearing);
  ImGui::Begin("Object configuration", &open, ImGuiWindowFlags_NoDocking);

  /* ------------ Left panel ---------*/
  /* -------------------------------- */
  ImGui::BeginChild("Left_ComponentList", ImVec2(200.0f, 0.0f), ImGuiChildFlags_Border | ImGuiChildFlags_ResizeX);
  Left_ComponentList();
  ImGui::EndChild();
  /* -------------------------------- */

  ImGui::SameLine();

  /* ------------ Right panel --------- */
  /* ---------------------------------- */
  ImGui::BeginGroup();
  ImGui::BeginChild("Right_ComponentView", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()));
  switch (tmp_compIdx)
  {
  case 0: /* Label component */
    Right_LabelView();
    break;

  case 1: /* Transform component */
    Right_TransformView(compToInclude[1].second);
    break;

  case 2: /* Model component */
    Right_ModelView(compToInclude[2].second);
    break;

  case 3: /* Light component */
    Right_LightView(compToInclude[3].second);
    break;
  }
  ImGui::EndChild();

  /* Confirm/Cancel buttons */
  static bool openDialog = false;
  char dialogMsg[128]{};
  if (ImGui::Button("Done"))
  {
    /* Check for errors */
    if (!Right_CheckValues(tmp_label, tmp_modelPath, dialogMsg, sizeof(dialogMsg)))
    {
      openDialog = true;
      ImGui::OpenPopup("Error dialog");
    }
    /* Everything is ok. Insert new object and reset temp values */
    else
    {
      Entity newObject = scene.CreateObject(tmp_label.value);
      if (compToInclude[1].second) /* +Add transform component */
      {
        auto& trans = newObject.AddComponent<Transform>();
        trans = tmp_transform;
      }
      if (compToInclude[2].second) /* +Add Model component */
      {
        newObject.AddComponent<Model>(tmp_modelPath);
      }
      if (compToInclude[3].second) /* +Add light component */
      {
        switch (tmp_lightIdx)
        {
        case 0: /* Dir light */
        {
          newObject.AddComponent<Light>(LightType::DIRECTIONAL);
          auto& dirLight = newObject.AddComponent<DirectionalLight>();
          dirLight = tmp_dirLight;
          break;
        }
        case 1: /* Point light */
        {
          newObject.AddComponent<Light>(LightType::POINT);
          auto& pointLight = newObject.AddComponent<PointLight>();
          pointLight = tmp_pointLight;
          break;
        }
        case 2: /* Spot light */
        {
          newObject.AddComponent<Light>(LightType::SPOT);
          auto& spotLight = newObject.AddComponent<SpotLight>();
          spotLight = tmp_spotLight;
          break;
        }
        }
      }

      ResetValues();
      open = false;
    }
  }
  ImGui::SameLine();
  if (ImGui::Button("Cancel"))
  {
    ResetValues();
    open = false;
  }

  if (openDialog)
    ShowDialog("Error dialog", dialogMsg, openDialog);

  ImGui::EndGroup();
  /* ---------------------------------- */

  ImGui::End();
}
static void Outliner_EntityMenuPopup(Scene& scene, Entity& objSelected)
{
  if (ImGui::BeginPopup("ObjectMenu"))
  {
    if (ImGui::MenuItem("Delete object"))
    {
      scene.DestroyObject(objSelected);
      objSelected.Invalidate();

      auto& shaderManager = ShaderManager::Get();
      shaderManager.GetProgramByName("Scene").Link();
      shaderManager.GetProgramByName("SceneShadows").Link();
      shaderManager.ResetProgramsUniforms();
    }
    ImGui::EndPopup();
  }
}

/* -------------------------- */
/*          PUBLIC            */
/* -------------------------- */

void GUI_RenderOutliner(bool& open, Scene& scene, Entity& objSelected)
{
  static bool openEntityConfig = false;

  ImGui::Begin("Outliner", &open);

  /* "+New entity" button */
  Outliner_NewEntityButton(openEntityConfig);
  ImGui::Separator();

  /* List all entities */
  Outliner_ListEntities(scene, objSelected);

  /* Entity configuration window */
  if (openEntityConfig)
    Outliner_CreateEntityWindow(openEntityConfig, scene);

  /* Display menu on right click object */
  Outliner_EntityMenuPopup(scene, objSelected);
  
  ImGui::End();
}

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

static constexpr const char* compList[] = {
  "Label",
  "Transform",
  "Model",
  "Light",
};
static constexpr const char* typeOfLights[] = {
  "Directional",
  "Point",
  "Spot"
};

static void Left_ComponentList(i32& compIdx, bool compToInclude[])
{
  for (i32 i = 0; i < std::size(compList); i++)
  {
    if (compToInclude[i])
    {
      ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.13f, 0.6f, 0.33f, 0.5f));
      ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.11f, 0.51f, 0.28f, 0.5f));
      ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.11f, 0.51f, 0.28f, 0.5f));
    }

    if (ImGui::Selectable(compList[i], compToInclude[i] || compIdx == i))
      compIdx = i;

    if (compToInclude[i])
      ImGui::PopStyleColor(3);

    ImGui::Spacing();
  }
}
static void Right_LabelView(Label& label)
{
  ImGui::SeparatorText("Label component");
  ImGui::BeginDisabled(true);
  bool check = true;
  ImGui::Checkbox("Add component", &check);
  ImGui::EndDisabled();
  ImGui::InputText("Value", label.value, sizeof(label.value));
}
static void Right_TransformView(bool& check, Transform& transform)
{
  ImGui::SeparatorText("Transform component");
  ImGui::Checkbox("Add component", &check);
  if (check)
  {
    ImGui::InputFloat3("Position", reinterpret_cast<f32*>(&transform.position));
    ImGui::InputFloat3("Scale", reinterpret_cast<f32*>(&transform.scale));
    ImGui::InputFloat3("Rotation", reinterpret_cast<f32*>(&transform.rotation));
  }
}
static void Right_ModelView(bool& check, String& strModelPath)
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
        strModelPath = path;
    }
    ImGui::SameLine();
    ImGui::BeginDisabled();
    ImGui::InputText("##model_path", strModelPath.data(), strModelPath.size());
    ImGui::EndDisabled();
  }
}
static void Right_LightView(bool& check, 
  i32& lightIdx,
  DirectionalLight& dirLight,
  PointLight& pointLight,
  SpotLight& spotLight
)
{
  ImGui::SeparatorText("Light component");
  ImGui::Checkbox("Add component", &check);
  if (check)
  {
    if (ImGui::BeginCombo("Type", typeOfLights[lightIdx]))
    {
      for (i32 i = 0; i < std::size(typeOfLights); i++)
        if (ImGui::Selectable(typeOfLights[i], lightIdx == i))
          lightIdx = i;

      ImGui::EndCombo();
    }

    switch (lightIdx)
    {
    case 0: /* Directional light */
      ImGui::InputFloat3("Color", reinterpret_cast<f32*>(&dirLight.color));
      ImGui::InputFloat("Diffuse intensity", reinterpret_cast<f32*>(&dirLight.diffuseIntensity));
      ImGui::InputFloat("Specular intensity", reinterpret_cast<f32*>(&dirLight.specularIntensity));
      ImGui::InputFloat3("Direction", reinterpret_cast<f32*>(&dirLight.direction));
      break;
    case 1: /* Point light */
      ImGui::InputFloat3("Color", reinterpret_cast<f32*>(&pointLight.color));
      ImGui::InputFloat("Diffuse intensity", reinterpret_cast<f32*>(&pointLight.diffuseIntensity));
      ImGui::InputFloat("Specular intensity", reinterpret_cast<f32*>(&pointLight.specularIntensity));
      ImGui::InputFloat3("Position", reinterpret_cast<f32*>(&pointLight.position));
      ImGui::InputFloat("Linear attenuation", reinterpret_cast<f32*>(&pointLight.attenuation.kl));
      ImGui::InputFloat("Quadratic attenuation", reinterpret_cast<f32*>(&pointLight.attenuation.kq));
      break;
    case 2: /* Spot light */
      ImGui::InputFloat3("Color", reinterpret_cast<f32*>(&spotLight.color));
      ImGui::InputFloat("Diffuse intensity", reinterpret_cast<f32*>(&spotLight.diffuseIntensity));
      ImGui::InputFloat("Specular intensity", reinterpret_cast<f32*>(&spotLight.specularIntensity));
      ImGui::InputFloat3("Direction", reinterpret_cast<f32*>(&spotLight.direction));
      ImGui::InputFloat3("Position", reinterpret_cast<f32*>(&spotLight.position));
      ImGui::InputFloat("Linear attenuation", reinterpret_cast<f32*>(&spotLight.attenuation.kl));
      ImGui::InputFloat("Quadratic attenuation", reinterpret_cast<f32*>(&spotLight.attenuation.kq));
      ImGui::InputFloat("Cutoff", reinterpret_cast<f32*>(&spotLight.cutOff));
      ImGui::InputFloat("Outer cutoff", reinterpret_cast<f32*>(&spotLight.outerCutOff));
      break;
    }

  }
}

static bool Right_CheckValues(StringView label,
  bool compToInclude[],
  StringView modelPath,
  char msg[],
  i32 size
)
{
  if (label.size() == 0)
  {
    std::format_to_n(msg, size, "Invalid label value (len=0)");
    return false;
  }
  if (compToInclude[2] && modelPath.empty())
  {
    std::format_to_n(msg, size, "Invalid model path (len=0)");
    return false;
  }
  return true;
}

static void ShowErrorDialog(StringView name, StringView message, bool& open)
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
static void ShowObjectConfigWindow(bool& open, Scene& scene)
{
  static bool compToInclude[] = 
  {
    true, /* Label component is always included */
    false,
    false,
    false,
  };
  static i32 compIdx = 0;
  static i32 lightIdx = 0;

  ImVec2 pos = ImGui::GetMainViewport()->GetCenter();
  pos.x -= 250.f;
  pos.y -= 150.f;
  ImGui::SetNextWindowPos(pos, ImGuiCond_Appearing);
  ImGui::SetNextWindowSize(ImVec2(500.f, 300.f), ImGuiCond_Appearing);
  ImGui::Begin("Object configuration", &open, ImGuiWindowFlags_NoDocking);

  /* ------------ Left panel ---------*/
  /* -------------------------------- */
  ImGui::BeginChild("Left_ComponentList", ImVec2(200.0f, 0.0f), ImGuiChildFlags_Border | ImGuiChildFlags_ResizeX);
  Left_ComponentList(compIdx, compToInclude);
  ImGui::EndChild();
  /* -------------------------------- */

  ImGui::SameLine();

  /* ------------ Right panel --------- */
  /* ---------------------------------- */
  static Label tmp_label{ "Object" };
  static Transform tmp_transform{};
  static String tmp_modelPath{};
  static DirectionalLight tmp_dirLight{};
  static PointLight tmp_pointLight{};
  static SpotLight tmp_spotLight{};

  ImGui::BeginGroup();
  ImGui::BeginChild("Right_ComponentView", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()));
  switch (compIdx)
  {
  case 0: /* Label component */
    Right_LabelView(tmp_label);
    break;

  case 1: /* Transform component */
    Right_TransformView(compToInclude[1], tmp_transform);
    break;
    
  case 2: /* Model component */
    Right_ModelView(compToInclude[2], tmp_modelPath);
    break;

  case 3: /* Light component */
    Right_LightView(compToInclude[3], lightIdx, tmp_dirLight, tmp_pointLight, tmp_spotLight);
    break;
  }
  ImGui::EndChild();

  /* Confirm/Cancel buttons */
  static bool openDialog = false;
  char dialogMsg[128]{};
  if (ImGui::Button("Done"))
  {
    /* Check for errors */
    if (!Right_CheckValues(tmp_label.value, compToInclude, tmp_modelPath, dialogMsg, sizeof(dialogMsg)))
    {
      openDialog = true;
      ImGui::OpenPopup("Error dialog");
    }
    /* Everything is ok. Insert new object and reset temp values */
    else
    {
      Entity newObject = scene.CreateObject(tmp_label.value);
      if (compToInclude[1]) /* +Add transform component */
      {
        auto& trans = newObject.AddComponent<Transform>();
        trans = tmp_transform;
      }
      if (compToInclude[2]) /* +Add Model component */
      {
        newObject.AddComponent<Model>(tmp_modelPath);
      }
      if (compToInclude[3]) /* +Add light component */
      {
        if (lightIdx == 0) /* Dir light */
        {
          auto& light = newObject.AddComponent<DirectionalLight>();
          light = tmp_dirLight;
        }
        else if (lightIdx == 1) /* Point light */
        {
          auto& light = newObject.AddComponent<PointLight>();
          light = tmp_pointLight;
        }
        else if (lightIdx == 2) /* Spot light */
        {
          auto& light = newObject.AddComponent<SpotLight>();
          light = tmp_spotLight;
        }
      }

      tmp_label.UpdateValue("Object");
      tmp_transform = Transform{};
      tmp_modelPath = String{};
      tmp_dirLight = DirectionalLight{};
      tmp_pointLight = PointLight{};
      tmp_spotLight = SpotLight{};
      compIdx = 0;
      lightIdx = 0;
      for (i32 i = 1; i < std::size(compToInclude); i++)
        compToInclude[i] = false;
      open = false;
    }
  }
  ImGui::SameLine();
  if (ImGui::Button("Cancel"))
    open = false;

  if (openDialog)
    ShowErrorDialog("Error dialog", dialogMsg, openDialog);

  ImGui::EndGroup();
  /* ---------------------------------- */
  
  ImGui::End();
}


/* -------------------------- */
/*          PUBLIC            */
/* -------------------------- */

void GUI_RenderOutliner(bool& open, Scene& scene, Entity& objSelected)
{
  static bool openObjectConfigWindow = false;

  ImGui::Begin("Outliner", &open);

  ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.f, 0.85f, 0.f, 1.f));
  ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
  if (ImGui::Button("+New object"))
    openObjectConfigWindow = true;
  ImGui::PopStyleVar();
  ImGui::PopStyleColor();

  ImGui::Separator();

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

  /* New object configuration window */
  if (openObjectConfigWindow)
    ShowObjectConfigWindow(openObjectConfigWindow, scene);

  /* Display menu on right click object */
  if (ImGui::BeginPopup("ObjectMenu"))
  {
    if (ImGui::MenuItem("+Add component"))
    {
      CONSOLE_DEBUG("TODO");
    }
    ImGui::Separator();
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
  
  ImGui::End();
}

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

static void Insp_Light_ComboAttenuation(Attenuation& destAtt)
{
  char currentAttRange[16]{};
  std::format_to_n(currentAttRange, sizeof(currentAttRange), "{}m", destAtt.range);
  if (ImGui::BeginCombo("##Range", currentAttRange))
  {
    char label[16]{};
    for (i32 i = 0; i < std::size(ATTENUATION_RANGES); i++)
    {
      const auto& attenuation = ATTENUATION_RANGES[i];
      std::fill_n(label, sizeof(label), 0);
      std::format_to_n(label, sizeof(label), "{}m", attenuation.range);
      if (ImGui::Selectable(label, destAtt.range == attenuation.range))
        destAtt = attenuation;
    }
    ImGui::EndCombo();
  }
}
static void Insp_DirectLight(GameObject& object, DirectionalLight& light)
{
  /* Create a table with two columns: one for the labels and one for input */
  if (ImGui::BeginTable("Light_Table", 2, ImGuiTableFlags_SizingFixedFit))
  {
    ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthFixed, 80.f);
    ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthStretch);

    /* 1° Row: view light type */
    ImGui::TableNextRow();
    ImGui::TableNextColumn(); /* First column: label */
    ImGui::Text("Type");
    ImGui::TableNextColumn(); /* Second column: combo */
    ImGui::BeginDisabled();
    if (ImGui::BeginCombo("##Type", "Directional"))
      ImGui::EndCombo();
    ImGui::EndDisabled();

    /* 2° Row: color input */
    ImGui::TableNextRow();
    ImGui::TableNextColumn(); /* First column: label */
    ImGui::Text("Color");
    ImGui::TableNextColumn(); /* Second column: input */
    ImGui::ColorEdit3("##Color", reinterpret_cast<f32*>(&light.color));
    
    /* 3° Row: intensity input */
    ImGui::TableNextRow();
    ImGui::TableNextColumn(); /* First column: label */
    ImGui::Text("Intensity");
    ImGui::TableNextColumn(); /* Second column: input */
    ImGui::SliderFloat("##Intensity", &light.intensity, 0.0f, 1.0f);

    /* 4° Row: direction input */
    ImGui::TableNextRow();
    ImGui::TableNextColumn(); /* First column: label */
    ImGui::Text("Direction");
    ImGui::TableNextColumn(); /* Second column: input */
    ImGui::DragFloat3("##Direction", reinterpret_cast<f32*>(&light.direction), 0.1f, -FLT_MAX, FLT_MAX);

    ImGui::EndTable();
  }

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
  /* Create a table with two columns: one for the labels and one for input */
  if (ImGui::BeginTable("Light_Table", 2, ImGuiTableFlags_SizingFixedFit))
  {
    ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthFixed, 80.f);
    ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthStretch);

    /* 1° Row: view light type */
    ImGui::TableNextRow();
    ImGui::TableNextColumn(); /* First column: label */
    ImGui::Text("Type");
    ImGui::TableNextColumn(); /* Second column: combo */
    ImGui::BeginDisabled();
    if (ImGui::BeginCombo("##Type", "Point"))
      ImGui::EndCombo();
    ImGui::EndDisabled();
    
    /* 2° Row: color input */
    ImGui::TableNextRow();
    ImGui::TableNextColumn(); /* First column: label */
    ImGui::Text("Color");
    ImGui::TableNextColumn(); /* Second column: input */
    ImGui::ColorEdit3("##Color", reinterpret_cast<f32*>(&light.color));

    /* 3° Row: diffuse input */
    ImGui::TableNextRow();
    ImGui::TableNextColumn(); /* First column: label */
    ImGui::Text("Intensity");
    ImGui::TableNextColumn(); /* Second column: input */
    ImGui::SliderFloat("##Intensity", &light.intensity, 0.0f, 1.0f);

    /* 4° Row: position input */
    ImGui::TableNextRow();
    ImGui::TableNextColumn(); /* First column: label */
    ImGui::Text("Position");
    ImGui::TableNextColumn(); /* Second column: input */
    ImGui::DragFloat3("##Position", reinterpret_cast<f32*>(&light.position), 0.1f, -FLT_MAX, FLT_MAX);

    /* 5° Row: attenuation combo */
    ImGui::TableNextRow();
    ImGui::TableNextColumn(); /* First column: label */
    ImGui::TextWrapped("Attenuation range");
    ImGui::TableNextColumn(); /* Second column: input */
    Insp_Light_ComboAttenuation(light.attenuation);
    ImGui::EndTable();
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
  /* Create a table with two columns: one for the labels and one for input */
  if (ImGui::BeginTable("Light_Table", 2, ImGuiTableFlags_SizingFixedFit))
  {
    ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthFixed, 80.f);
    ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthStretch);

    /* 1° Row: view light type */
    ImGui::TableNextRow();
    ImGui::TableNextColumn(); /* First column: label */
    ImGui::Text("Type");
    ImGui::TableNextColumn(); /* Second column: combo */
    ImGui::BeginDisabled();
    if (ImGui::BeginCombo("##Type", "Spot"))
      ImGui::EndCombo();
    ImGui::EndDisabled();

    /* 2° Row: color input */
    ImGui::TableNextRow();
    ImGui::TableNextColumn(); /* First column: label */
    ImGui::Text("Color");
    ImGui::TableNextColumn(); /* Second column: input */
    ImGui::ColorEdit3("##Color", reinterpret_cast<f32*>(&light.color));

    /* 3° Row: diffuse input */
    ImGui::TableNextRow();
    ImGui::TableNextColumn(); /* First column: label */
    ImGui::Text("Intensity");
    ImGui::TableNextColumn(); /* Second column: input */
    ImGui::SliderFloat("##Intensity", &light.intensity, 0.0f, 1.0f);

    /* 4° Row: direction input */
    ImGui::TableNextRow();
    ImGui::TableNextColumn(); /* First column: label */
    ImGui::Text("Direction");
    ImGui::TableNextColumn(); /* Second column: input */
    ImGui::DragFloat3("##Direction", reinterpret_cast<f32*>(&light.direction), 0.1f, -FLT_MAX, FLT_MAX);

    /* 5° Row: position input */
    ImGui::TableNextRow();
    ImGui::TableNextColumn(); /* First column: label */
    ImGui::Text("Position");
    ImGui::TableNextColumn(); /* Second column: input */
    ImGui::DragFloat3("##Position", reinterpret_cast<f32*>(&light.position), 0.1f, -FLT_MAX, FLT_MAX);

    /* 6° Row: attenuation combo */
    ImGui::TableNextRow();
    ImGui::TableNextColumn(); /* First column: label */
    ImGui::TextWrapped("Attenuation range");
    ImGui::TableNextColumn(); /* Second column: input */
    Insp_Light_ComboAttenuation(light.attenuation);

    /* 7° Row: inner cutoff input */
    ImGui::TableNextRow();
    ImGui::TableNextColumn(); /* First column: label */
    ImGui::Text("Inner cutoff");
    ImGui::TableNextColumn(); /* Second column: input */
    ImGui::SliderFloat("##Inner_Cutoff", &light.cutOff, 1.0f, light.outerCutOff);

    /* 8° Row: outer cutoff input */
    ImGui::TableNextRow();
    ImGui::TableNextColumn(); /* First column: label */
    ImGui::Text("Outer cutoff");
    ImGui::TableNextColumn(); /* Second column: input */
    ImGui::SliderFloat("##Outer_Cutoff", &light.outerCutOff, light.cutOff, 45.0f);

    ImGui::EndTable();
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
static void Insp_Transform_TableRow(StringView label, vec3f& values, f32 resetValue)
{
  ImGui::PushID(label.data());

  /* First column: label */
  ImGui::TableNextColumn();
  ImGui::Text(label.data());

  /* First column: inputs */
  ImGui::TableNextColumn();
  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

  f32 lineHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
  ImVec2 buttonSize{ lineHeight + 3.0f, lineHeight };
  f32 itemWidth = ImGui::CalcItemWidth() / 3.0f;

  /* X */
  ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
  ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.7f, 0.05f, 0.1f, 1.0f });
  if (ImGui::Button("X", buttonSize))
    values.x = resetValue;
  ImGui::PopStyleColor(3);
  ImGui::SameLine();
  ImGui::SetNextItemWidth(itemWidth);
  
  ImGui::DragFloat("##X", &values.x, 0.1f);
  ImGui::SameLine();

  /* Y */
  ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
  ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.6f, 0.1f, 1.0f });
  if (ImGui::Button("Y", buttonSize))
    values.y = resetValue;
  ImGui::PopStyleColor(3);
  ImGui::SameLine();
  ImGui::SetNextItemWidth(itemWidth);
  ImGui::DragFloat("##Y", &values.y, 0.1f);
  ImGui::SameLine();

  /* Z */
  ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
  ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.05f, 0.2f, 0.7f, 1.0f });
  if (ImGui::Button("Z", buttonSize))
    values.z = resetValue;
  ImGui::PopStyleColor(3);
  ImGui::SameLine();
  ImGui::SetNextItemWidth(itemWidth);
  ImGui::DragFloat("##Z", &values.z, 0.1f);
  ImGui::PopStyleVar();

  ImGui::PopID();
}
static void Insp_Transform(GameObject& object, Transform& transform)
{
  /* Create a table with two columns: one for the labels and one for inputs */
  if (ImGui::BeginTable("Transform_Table", 2, ImGuiTableFlags_SizingFixedFit))
  {
    ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthFixed, 80.f);
    ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthStretch);

    /* First row: position */
    ImGui::TableNextRow();
    Insp_Transform_TableRow("Position", transform.position, 0.f);

    /* First row: Rotation */
    ImGui::TableNextRow();
    Insp_Transform_TableRow("Rotation", transform.rotation, 0.f);

    /* First row: Scale */
    ImGui::TableNextRow();
    Insp_Transform_TableRow("Scale", transform.scale, 1.f);

    transform.UpdateTransformation();

    ImGui::EndTable();
  }

  ImGui::SeparatorText("Advanced");
  ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.55f, 0.f, 0.f, 0.5f));
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.45f, 0.f, 0.f, 0.5f));
  ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.45f, 0.f, 0.f, 0.5f));
  if (ImGui::Button("Remove component##transform"))
    object.RemoveComponent<Transform>();
  ImGui::PopStyleColor(3);
}
static void Insp_Model_MeshMaterialRow(StringView label, Texture2D*& matTexture, Texture2D& defaultTex)
{
  auto& texManager = TextureManager::Get();
  
  const bool noTexture = (matTexture->strPath.at(0) == '#');
  
  /* First column: label */
  ImGui::TableNextColumn();
  ImGui::Text(label.data());

  /* Second column: combo */
  ImGui::TableNextColumn();
  ImGui::SetNextItemWidth(ImGui::GetColumnWidth());

  char comboId[32]{}; // "##Diffuse"
  std::format_to_n(comboId, sizeof(comboId), "##{}", label.data());

  if (ImGui::BeginCombo(comboId, (noTexture ? "Select texture" : matTexture->strPath.c_str())))
  {
    for (auto& [key, texture] : texManager.GetTextures())
      if (key.at(0) != '#' && ImGui::Selectable(key.c_str(), texture.Compare(*matTexture)))
        matTexture = &texture;

    ImGui::EndCombo();
  }

  /* Third column: reset button */
  ImGui::TableNextColumn();
  if (!noTexture)
  {
    char buttonID[32]{}; // "Reset##Diffuse"
    std::format_to_n(buttonID, sizeof(buttonID), "Reset##{}", label.data());

    static const auto& resetIcon = texManager.GetIconByPath(GetIconsPath() / "reset-arrow-16.png");
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.f,0.f,0.f,0.f });
    if (ImGui::ImageButton(buttonID, reinterpret_cast<void*>(resetIcon.id), ImVec2(16.f, 16.f)))
      matTexture = &defaultTex;
    ImGui::PopStyleColor();
  }
}
static void Insp_Model(GameObject& object, Model& model)
{
  /* View the model path */
  ImGui::TextWrapped("Model: %s", model.strPath.c_str());

  /* View model meshes with a tree */
  if (ImGui::TreeNode("Material"))
  {
    const i64 numMeshes = model.meshes.size();
    for (i32 i = 0; i < numMeshes; i++)
    {
      auto& mesh = model.meshes.at(i);
      Material& material = mesh.material;
      TextureManager& texManager = TextureManager::Get();

      char meshName[16]{};
      std::format_to_n(meshName, sizeof(meshName), "Mesh_{}", i);
      if (ImGui::TreeNode(meshName))
      {
        if (ImGui::BeginTable("TextureTable", 3, ImGuiTableFlags_SizingFixedFit))
        {
          ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthFixed, 48.0f);
          ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthStretch);
          ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthFixed, 24.0f);

          /* Diffuse row */
          ImGui::TableNextRow();
          Insp_Model_MeshMaterialRow("Diffuse", material.diffuse, texManager.GetDefaultDiffuse());
          
          /* Specular row */
          ImGui::TableNextRow();
          Insp_Model_MeshMaterialRow("Specular", material.specular, texManager.GetDefaultSpecular());
          
          /* Normal row */
          ImGui::TableNextRow();
          Insp_Model_MeshMaterialRow("Normal", material.normal, texManager.GetDefaultNormal());

          /* Height row */
          ImGui::TableNextRow();
          Insp_Model_MeshMaterialRow("Height", material.height, texManager.GetDefaultHeight());

          ImGui::EndTable();
        }
        ImGui::TreePop();
      }
    }

    ImGui::TreePop();
  }

  ImGui::SeparatorText("Advanced");
  ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.55f, 0.f, 0.f, 0.5f));
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.45f, 0.f, 0.f, 0.5f));
  ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.45f, 0.f, 0.f, 0.5f));
  if (ImGui::Button("Remove component##model"))
    object.RemoveComponent<Model>();
  ImGui::PopStyleColor(3);
}
static void Insp_Tag(Tag& tag)
{
  /* Create a table with two columns: one for the labels and one for input */
  if (ImGui::BeginTable("Tag_Table", 2, ImGuiTableFlags_SizingFixedFit))
  {
    ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthFixed, 25.f);
    ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthStretch);

    ImGui::TableNextRow();

    /* First column: label */
    ImGui::TableNextColumn();
    ImGui::Text("Tag");

    /* First column: input */
    ImGui::TableNextColumn();
    char buffer[64]{};
    std::format_to_n(buffer, sizeof(buffer), "{}", tag.value);

    if (ImGui::InputText("##Value", buffer, sizeof(buffer), ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_EnterReturnsTrue))
    {
      if (std::strlen(buffer) > 0)
        tag.UpdateValue(buffer);
    }

    ImGui::EndTable();
  }
}

static void Insp_ListAllComponents(GameObject& object)
{
  if (ImGui::CollapsingHeader("Tag"))
  {
    auto* tag = object.GetComponent<Tag>();
    Insp_Tag(*tag);
  }

  if (auto* transform = object.GetComponent<Transform>())
  {
    if (ImGui::CollapsingHeader("Transform"))
      Insp_Transform(object, *transform);
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


void GUI_RenderInspector(bool& open, GameObject& object)
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
    Insp_ListAllComponents(object);
  }

  /* Open popup on click to "+New component" button */
  Insp_NewComponentPopup(object);

  ImGui::End();
}

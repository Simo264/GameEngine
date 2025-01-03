#include "Core/Core.hpp"

#include "Engine/Scene.hpp"
#include "Engine/ECS/ECS.hpp"
#include "Engine/Subsystems/TextureManager.hpp"
#include "Engine/Subsystems/ShaderManager.hpp"
#include "Engine/Filesystem/Dialog.hpp"

#include <imgui/imgui.h>
#include <imgui/ImGuizmo.h>


// ------------------------------------------
//                  PRIVATE                  
// ------------------------------------------

static bool ButtonCentered(const char* label, ImVec2 size)
{
  f32 avail = ImGui::GetContentRegionAvail().x;
  f32 off = (avail - size.x) * 0.5f;
  if (off > 0.0f)
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);

  return ImGui::Button(label, size);
}
static void Insp_Tag(Tag& tag)
{
  // Create a table with two columns: one for the labels and one for input
  if (ImGui::BeginTable("Tag_Table", 2, ImGuiTableFlags_SizingFixedFit))
  {
    ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthFixed, 25.f);
    ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthStretch);

    ImGui::TableNextRow();

    // First column: label
    ImGui::TableNextColumn();
    ImGui::Text("Tag");

    // First column: input
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
  // Create a table with two columns: one for the labels and one for input
  if (ImGui::BeginTable("Light_Table", 2, ImGuiTableFlags_SizingFixedFit))
  {
    ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthFixed, 80.f);
    ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthStretch);

    // 1° Row: view light type
    ImGui::TableNextRow();
    ImGui::TableNextColumn(); // First column: label
    ImGui::Text("Type");
    ImGui::TableNextColumn(); // Second column: combo
    ImGui::BeginDisabled();
    if (ImGui::BeginCombo("##Type", "Directional"))
      ImGui::EndCombo();
    ImGui::EndDisabled();

    // 2° Row: color input
    ImGui::TableNextRow();
    ImGui::TableNextColumn(); // First column: label
    ImGui::Text("Color");
    ImGui::TableNextColumn(); // Second column: input
    ImGui::ColorEdit3("##Color", reinterpret_cast<f32*>(&light.color));
    
    // 3° Row: intensity input
    ImGui::TableNextRow();
    ImGui::TableNextColumn(); // First column: label
    ImGui::Text("Intensity");
    ImGui::TableNextColumn(); // Second column: input
    ImGui::SliderFloat("##Intensity", &light.intensity, 0.0f, 1.0f);

    // 4° Row: direction input
    ImGui::TableNextRow();
    ImGui::TableNextColumn(); // First column: label
    ImGui::Text("Direction");
    ImGui::TableNextColumn(); // Second column: input
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

    ShaderManager& shaderManager = ShaderManager::Get();
    auto& shaderScene = shaderManager.GetProgramByName("Scene");
    auto& shaderSceneShadows = shaderManager.GetProgramByName("SceneShadows");
    shaderScene.SetUniform1f("u_directionalLight.intensity", 0.f);
    shaderSceneShadows.SetUniform1f("u_directionalLight.intensity", 0.f);
  }
  ImGui::PopStyleColor(3);
}
static void Insp_PointLight(GameObject& object, PointLight& light)
{
  // Create a table with two columns: one for the labels and one for input
  if (ImGui::BeginTable("Light_Table", 2, ImGuiTableFlags_SizingFixedFit))
  {
    ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthFixed, 80.f);
    ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthStretch);

    // 1° Row: view light type
    ImGui::TableNextRow();
    ImGui::TableNextColumn(); // First column: label
    ImGui::Text("Type");
    ImGui::TableNextColumn(); // Second column: combo
    ImGui::BeginDisabled();
    if (ImGui::BeginCombo("##Type", "Point"))
      ImGui::EndCombo();
    ImGui::EndDisabled();
    
    // 2° Row: color input
    ImGui::TableNextRow();
    ImGui::TableNextColumn(); // First column: label
    ImGui::Text("Color");
    ImGui::TableNextColumn(); // Second column: input
    ImGui::ColorEdit3("##Color", reinterpret_cast<f32*>(&light.color));

    // 3° Row: diffuse input
    ImGui::TableNextRow();
    ImGui::TableNextColumn(); // First column: label
    ImGui::Text("Intensity");
    ImGui::TableNextColumn(); // Second column: input
    ImGui::SliderFloat("##Intensity", &light.intensity, 0.0f, 1.0f);

    // 4° Row: position input
    ImGui::TableNextRow();
    ImGui::TableNextColumn(); // First column: label
    ImGui::Text("Position");
    ImGui::TableNextColumn(); // Second column: input
    ImGui::DragFloat3("##Position", reinterpret_cast<f32*>(&light.position), 0.1f, -FLT_MAX, FLT_MAX);

    // 5° Row: attenuation combo
    ImGui::TableNextRow();
    ImGui::TableNextColumn(); // First column: label
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

    ShaderManager& shaderManager = ShaderManager::Get();
    auto& shaderScene = shaderManager.GetProgramByName("Scene");
    auto& shaderSceneShadows = shaderManager.GetProgramByName("SceneShadows");
    shaderScene.SetUniform1f("u_pointLight.intensity", 0.f);
    shaderSceneShadows.SetUniform1f("u_pointLight.intensity", 0.f);
  }
  ImGui::PopStyleColor(3);
}
static void Insp_SpotLight(GameObject& object, SpotLight& light)
{
  // Create a table with two columns: one for the labels and one for input
  if (ImGui::BeginTable("Light_Table", 2, ImGuiTableFlags_SizingFixedFit))
  {
    ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthFixed, 80.f);
    ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthStretch);

    // 1° Row: view light type
    ImGui::TableNextRow();
    ImGui::TableNextColumn(); // First column: label
    ImGui::Text("Type");
    ImGui::TableNextColumn(); // Second column: combo
    ImGui::BeginDisabled();
    if (ImGui::BeginCombo("##Type", "Spot"))
      ImGui::EndCombo();
    ImGui::EndDisabled();

    // 2° Row: color input
    ImGui::TableNextRow();
    ImGui::TableNextColumn(); // First column: label
    ImGui::Text("Color");
    ImGui::TableNextColumn(); // Second column: input
    ImGui::ColorEdit3("##Color", reinterpret_cast<f32*>(&light.color));

    // 3° Row: diffuse input
    ImGui::TableNextRow();
    ImGui::TableNextColumn(); // First column: label
    ImGui::Text("Intensity");
    ImGui::TableNextColumn(); // Second column: input
    ImGui::SliderFloat("##Intensity", &light.intensity, 0.0f, 1.0f);

    // 4° Row: direction input
    ImGui::TableNextRow();
    ImGui::TableNextColumn(); // First column: label
    ImGui::Text("Direction");
    ImGui::TableNextColumn(); // Second column: input
    ImGui::DragFloat3("##Direction", reinterpret_cast<f32*>(&light.direction), 0.1f, -FLT_MAX, FLT_MAX);

    // 5° Row: position input
    ImGui::TableNextRow();
    ImGui::TableNextColumn(); // First column: label
    ImGui::Text("Position");
    ImGui::TableNextColumn(); // Second column: input
    ImGui::DragFloat3("##Position", reinterpret_cast<f32*>(&light.position), 0.1f, -FLT_MAX, FLT_MAX);

    // 6° Row: attenuation combo
    ImGui::TableNextRow();
    ImGui::TableNextColumn(); // First column: label
    ImGui::TextWrapped("Attenuation range");
    ImGui::TableNextColumn(); // Second column: input
    Insp_Light_ComboAttenuation(light.attenuation);

    // 7° Row: inner cutoff input
    ImGui::TableNextRow();
    ImGui::TableNextColumn(); // First column: label
    ImGui::Text("Inner cutoff");
    ImGui::TableNextColumn(); // Second column: input
    ImGui::SliderFloat("##Inner_Cutoff", &light.cutOff, 1.0f, light.outerCutOff);

    // 8° Row: outer cutoff input
    ImGui::TableNextRow();
    ImGui::TableNextColumn(); // First column: label
    ImGui::Text("Outer cutoff");
    ImGui::TableNextColumn(); // Second column: input
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

    ShaderManager& shaderManager = ShaderManager::Get();
    auto& shaderScene = shaderManager.GetProgramByName("Scene");
    auto& shaderSceneShadows = shaderManager.GetProgramByName("SceneShadows");
    shaderScene.SetUniform1f("u_spotLight.intensity", 0.f);
    shaderSceneShadows.SetUniform1f("u_spotLight.intensity", 0.f);
  }
  ImGui::PopStyleColor(3);
}
static void Insp_Transform_TableRow(StringView label, vec3f& values, f32 resetValue)
{
  ImGui::PushID(label.data());

  // First column: label
  ImGui::TableNextColumn();
  ImGui::Text(label.data());

  // First column: inputs
  ImGui::TableNextColumn();
  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

  f32 lineHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
  ImVec2 buttonSize{ lineHeight + 3.0f, lineHeight };
  f32 itemWidth = ImGui::CalcItemWidth() / 3.0f;

  // X value
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

  // Y value
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

  // Z value
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
  // Create a table with two columns: one for the labels and one for inputs
  if (ImGui::BeginTable("Transform_Table", 2, ImGuiTableFlags_SizingFixedFit))
  {
    ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthFixed, 80.f);
    ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthStretch);

    // First row: position
    ImGui::TableNextRow();
    Insp_Transform_TableRow("Position", transform.position, 0.f);

    // First row: Rotation
    ImGui::TableNextRow();
    Insp_Transform_TableRow("Rotation", transform.rotation, 0.f);

    // First row: Scale
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
static void Insp_MaterialRow(StringView label, Texture2D*& matTexture, Texture2D& defaultTex)
{
  auto& texManager = TextureManager::Get();
  const bool noTexture = (matTexture->path.string().at(0) == '#');
  
  // First column: label
  ImGui::TableNextColumn();
  ImGui::Text(label.data());

  // Second column: combo
  ImGui::TableNextColumn();
  ImGui::SetNextItemWidth(ImGui::GetColumnWidth());

  char comboId[32]{}; // "##Diffuse"
  std::format_to_n(comboId, sizeof(comboId), "##{}", label.data());

  if (ImGui::BeginCombo(comboId, (noTexture ? "Select texture" : matTexture->path.string().c_str())))
  {
    for (auto& [path, texture] : texManager.GetTextures())
    {
      String strPath = path.string();
      if (strPath.at(0) != '#' && ImGui::Selectable(strPath.c_str(), texture.Compare(*matTexture)))
        matTexture = const_cast<Texture2D*>(&texture);
    }
    ImGui::EndCombo();
  }

  // Third column: reset button
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
static void Insp_StaticMesh(GameObject& object, StaticMesh& staticMesh)
{
  ImGui::Text("Path: %s", staticMesh.path.string().c_str());
  ImGui::Text("Nr meshes: %d", staticMesh.meshes.size());
  ImGui::Text("Total vertices: %d", staticMesh.TotalVertices());
  ImGui::Text("Total indices: %d", staticMesh.TotalIndices());

  // View meshes with a tree
  if (ImGui::TreeNode("Material"))
  {
    for (i32 i = 0; i < staticMesh.meshes.size(); i++)
    {
      auto& mesh = staticMesh.meshes.at(i);
      Material& material = mesh.material;
      TextureManager& texManager = TextureManager::Get();

      char label[16]{};
      std::format_to_n(label, sizeof(label), "Mesh_{}", i+1);
      if (ImGui::TreeNode(label))
      {
        if (ImGui::BeginTable("TextureTable", 3, ImGuiTableFlags_SizingFixedFit))
        {
          ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthFixed, 48.0f);
          ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthStretch);
          ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthFixed, 24.0f);

          // Diffuse row
          ImGui::TableNextRow();
          Insp_MaterialRow("Diffuse", material.diffuse, texManager.GetDefaultDiffuse());
          
          // Specular row
          ImGui::TableNextRow();
          Insp_MaterialRow("Specular", material.specular, texManager.GetDefaultSpecular());
          
          // Normal row
          ImGui::TableNextRow();
          Insp_MaterialRow("Normal", material.normal, texManager.GetDefaultNormal());

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
    object.RemoveComponent<StaticMesh>();
  ImGui::PopStyleColor(3);
}

static void Insp_SkeletonMesh_BoneTree(const BoneNode& bone)
{
  if (ImGui::TreeNode(bone.name)) 
  {
    for (const BoneNode& child : bone.children)
      Insp_SkeletonMesh_BoneTree(child);

    ImGui::TreePop();
  }
}
static void Insp_SkeletonMesh(GameObject& object, SkeletonMesh& skeleton)
{
  ImGui::Text("Path: %s", skeleton.path.string().c_str());
  ImGui::Text("Nr meshes: %d", skeleton.meshes.size());
  ImGui::Text("Total vertices: %d", skeleton.TotalVertices());
  ImGui::Text("Total indices: %d", skeleton.TotalIndices());
  ImGui::Text("Nr bones: %d", skeleton.bones.size());
  Insp_SkeletonMesh_BoneTree(skeleton.rootNode);

  if (ImGui::TreeNode("Material"))
  {
    for (i32 i = 0; i < skeleton.meshes.size(); i++)
    {
      auto& mesh = skeleton.meshes.at(i);
      Material& material = mesh.material;
      TextureManager& texManager = TextureManager::Get();

      char label[16]{};
      std::format_to_n(label, sizeof(label), "Mesh_{}", i+1);
      if (ImGui::TreeNode(label))
      {
        if (ImGui::BeginTable("TextureTable", 3, ImGuiTableFlags_SizingFixedFit))
        {
          ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthFixed, 48.0f);
          ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthStretch);
          ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthFixed, 24.0f);

          //* Diffuse row
          ImGui::TableNextRow();
          Insp_MaterialRow("Diffuse", material.diffuse, texManager.GetDefaultDiffuse());

          //* Specular row
          ImGui::TableNextRow();
          Insp_MaterialRow("Specular", material.specular, texManager.GetDefaultSpecular());

          // Normal row 
          ImGui::TableNextRow();
          Insp_MaterialRow("Normal", material.normal, texManager.GetDefaultNormal());

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
    object.RemoveComponent<SkeletonMesh>();
  ImGui::PopStyleColor(3);
}
static void Insp_Animator(GameObject& object, Animator& animator)
{
  auto& animations = animator.AnimationsMap();
  ImGui::Text("Nr animations: %d", animations.size());

  if (ImGui::Button("+Load new animation"))
  {
    const char* filter[] = { "*.obj", "*.glb", "*.gltf", "*.fbx" };
    u32 numFilters = sizeof(filter) / sizeof(filter[0]);
    fs::path path = OpenFileDialog(numFilters, filter, "Static mesh file", false);
    if (!path.empty())
    {
      auto [name, anim] = animator.InsertAnimation(path);
      if (!name)
        CONSOLE_WARN("Error on loading new animation");
    }
  }

  auto [animNameTarget, animTarget] = animator.AttachedAnimation();
  ImGui::Text("Current animation: %s", (animNameTarget == nullptr ? "None" : animNameTarget));
  if (ImGui::BeginCombo("Animation list", (animNameTarget == nullptr ? "Select animation" : animNameTarget)))
  {
    for (const auto& [name, anim] : animations)
      if (ImGui::Selectable(name.c_str(), animNameTarget == name.c_str()))
        animator.AttachAnimation(name);

    ImGui::EndCombo();
  }

  if (animNameTarget)
  {
    Texture2D& playIcon = TextureManager::Get().GetIconByPath(GetIconsPath() / "play-button-32.png");
    Texture2D& pauseIcon = TextureManager::Get().GetIconByPath(GetIconsPath() / "pause-button-32.png");
    Texture2D& restartIcon = TextureManager::Get().GetIconByPath(GetIconsPath() / "restart-button-32.png");

    if (ImGui::ImageButton(reinterpret_cast<void*>(playIcon.id), ImVec2(16, 16)))
      animator.PlayAnimation();
    
    ImGui::SameLine();
    
    if (ImGui::ImageButton(reinterpret_cast<void*>(pauseIcon.id), ImVec2(16, 16)))
      animator.PauseAnimation();
    
    ImGui::SameLine();

    if (ImGui::ImageButton(reinterpret_cast<void*>(restartIcon.id), ImVec2(16, 16)))
      animator.RestartAnimation();
    
    ImGui::SameLine();
    
    if (ImGui::Button("Unbind animation"))
      animator.DetachAnimation();

    f32 duration = animTarget->Duration();
    f32 currentTime = animator.CurrentTime();
    f32 progression = currentTime / duration;
    char label[8]{};
    std::format_to_n(label, sizeof(label), "{}%", static_cast<u32>(progression*100));

    ImGui::Text("Animation progress:");
    ImGui::ProgressBar(progression, ImVec2(-1, 0), label);
  }
}

static void Insp_AddLightComponent(GameObject& object)
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
}
static void Insp_AddStaticMeshComponent(GameObject& object)
{
  ImGui::BeginChild("StaticMesh_Child", ImVec2(300, 100));
  static fs::path path{};
  if (ImGui::Button("Open static mesh file"))
  {
    const char* filter[] = { "*.obj", "*.glb", "*.gltf", "*.fbx" };
    u32 numFilters = sizeof(filter) / sizeof(filter[0]);
    path = OpenFileDialog(numFilters, filter, "Static mesh file", false);
  }
  String pathS = path.string();

  ImGui::SameLine();
  ImGui::InputText("##", pathS.data(), pathS.size(), ImGuiInputTextFlags_ReadOnly);

  if (!path.empty())
  {
    if (ImGui::Button("Ok"))
    {
      object.AddComponent<StaticMesh>(path);
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
}
static void Insp_AddSkeletonComponent(GameObject& object)
{
  ImGui::BeginChild("SkeletonMesh_Child", ImVec2(300, 100));
  static fs::path path{};
  if (ImGui::Button("Open skeleton mesh file"))
  {
    const char* filter[] = { "*.obj", "*.glb", "*.gltf", "*.fbx" };
    u32 numFilters = sizeof(filter) / sizeof(filter[0]);
    path = OpenFileDialog(numFilters, filter, "Skeleton mesh file", false);
  }
  String pathS = path.string();

  ImGui::SameLine();
  ImGui::InputText("##", pathS.data(), pathS.size(), ImGuiInputTextFlags_ReadOnly);

  if (!path.empty())
  {
    if (ImGui::Button("Ok"))
    {
      object.AddComponent<SkeletonMesh>(path);
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
}

static void Insp_ListAllComponents(GameObject& object)
{
  if (ImGui::CollapsingHeader("Tag"))
  {
    Tag* tag = object.GetComponent<Tag>();
    Insp_Tag(*tag);
  }
  if (Transform* transform = object.GetComponent<Transform>())
  {
    if (ImGui::CollapsingHeader("Transform"))
      Insp_Transform(object, *transform);
  }
  if (StaticMesh* staticMesh = object.GetComponent<StaticMesh>())
  {
    if (ImGui::CollapsingHeader("StaticMesh"))
      Insp_StaticMesh(object, *staticMesh);
  }
  if (SkeletonMesh* skeleton = object.GetComponent<SkeletonMesh>())
  {
    if (ImGui::CollapsingHeader("SkeletonMesh"))
      Insp_SkeletonMesh(object, *skeleton);
  }
  if (Animator* animator = object.GetComponent<Animator>())
  {
    if (ImGui::CollapsingHeader("Animator"))
      Insp_Animator(object, *animator);
  }
  if (Light* light = object.GetComponent<Light>())
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
    // Add Transform component
    // --------------------------------
    if (object.HasComponent<Transform>()) // Component already present, disabled
      ImGui::Selectable("Transform", false, ImGuiSelectableFlags_Disabled);
    else if (ImGui::Selectable("Transform"))
      object.AddComponent<Transform>();

    ImGui::Spacing();

    // Add Light component
    // --------------------------------
    if (object.HasComponent<Light>()) // Component already present, disabled
      ImGui::Selectable("Light", false, ImGuiSelectableFlags_Disabled);
    else if (ImGui::BeginMenu("Light"))
    {
      Insp_AddLightComponent(object);
      ImGui::EndMenu();
    }

    ImGui::Spacing();

    // Add StaticMesh component
    // --------------------------------
    if (object.HasComponent<StaticMesh>()) // Component already present, disabled
      ImGui::Selectable("StaticMesh", false, ImGuiSelectableFlags_Disabled);
    else if (ImGui::BeginMenu("StaticMesh"))
    {
      Insp_AddStaticMeshComponent(object);
      ImGui::EndMenu();
    }

    ImGui::Spacing();

    // Add SkeletonMesh component
    // --------------------------------
    if (object.HasComponent<SkeletonMesh>()) // Component already present, disabled
      ImGui::Selectable("SkeletonMesh", false, ImGuiSelectableFlags_Disabled);
    else if (ImGui::BeginMenu("SkeletonMesh"))
    {
      Insp_AddSkeletonComponent(object);
      ImGui::EndMenu();
    }

    // Add Animator component
    // --------------------------------
    if (object.HasComponent<Animator>() || !object.HasComponent<SkeletonMesh>()) // Component already present, disabled
      ImGui::Selectable("Animator", false, ImGuiSelectableFlags_Disabled);
    else if (ImGui::Selectable("Animator"))
    {
      SkeletonMesh* skeleton = object.GetComponent<SkeletonMesh>();
      object.AddComponent<Animator>(*skeleton);
    }

    ImGui::EndPopup();
  }
}


// ------------------------------------------
//                    PUBLIC                 
// ------------------------------------------


void GUI_RenderInspector(bool& open, GameObject& object)
{
  ImGui::Begin("Inspector", &open);
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

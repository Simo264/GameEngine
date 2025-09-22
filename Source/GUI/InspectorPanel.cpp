#include "InspectorPanel.hpp"
#include "Engine/Scene.hpp"

#include "Engine/Managers/TexturesManager.hpp"

#include <imgui.h>

using namespace Components;

InspectorPanel::InspectorPanel() : isOpen{ true }
{
  auto& texManager = TexturesManager::GetInstance();
  _resetIcon = texManager.GetOrCreateIcon("reset-arrow-16.png");
}

void InspectorPanel::Render(StringView windowName, Entity entity)
{
  if (!isOpen)
    return;

  ImGui::Begin(windowName.data(), &isOpen);
  if (entity.IsValid())
  {
    //auto btnWidth = ImGui::GetContentRegionAvail().x - 32.f;
    //if (__ButtonCentered("+Add component", Vec2I(btnWidth, 26.f)))
    //  ImGui::OpenPopup("NewComponent_Popup");
    //ImGui::Spacing();
    //ImGui::Separator();
    //ImGui::Spacing();

    __ListAllComponents(entity);
  }

  //__NewComponentPopup(entity);

  ImGui::End();
}

void InspectorPanel::__ListAllComponents(Entity entity)
{
  auto archetypeComponent = entity.GetComponent<ArchetypeIdentifier>();
  __RenderArchetype(archetypeComponent->archetypeId, entity.GetArchetype()->GetName());

  ImGui::Spacing();
  ImGui::Separator();

  auto tagComponent = entity.GetComponent<Tag>();
  __RenderTagComponent(*tagComponent);

  ImGui::Spacing();
  ImGui::Separator();

  if (auto transform = entity.GetComponent<Transform>())
  {
    if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick))
    {
      __RenderTransformComponent(*transform);
    }
  }
  if (auto camera = entity.GetComponent<Camera>())
  {
    if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick))
    {
      __RenderCameraComponent(*camera);
    }
  }
  if (auto light = entity.GetComponent<Light>())
  {
    if (ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick))
    {
      // ... (implementazione per il componente Light)
    }
  }
  if (auto staticmesh = entity.GetComponent<StaticMesh>())
  {
    if (ImGui::CollapsingHeader("Static mesh", ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick))
    {
      // ... (implementazione per il componente Light)
    }
  }
}

void InspectorPanel::__RenderArchetype(u32 archetypeId, StringView archetypeName) const
{
  ImGui::Text("Archetype: %s (ID %d)", archetypeName.data(), archetypeId);
}
void InspectorPanel::__RenderTagComponent(Tag& tagComponent) const
{
  auto tagBuffer = Array<char, 32>{};
  auto currentTag = StringView(tagComponent.value.data());
  auto lengthToCopy = std::min(currentTag.size(), sizeof(tagBuffer) - 1);
  std::copy_n(currentTag.begin(), lengthToCopy, tagBuffer.data());
  tagBuffer[currentTag.size()] = '\0';
  if (ImGui::InputText("##Tag", tagBuffer.data(), sizeof(tagBuffer), ImGuiInputTextFlags_EnterReturnsTrue))
  {
    auto newTagValue = StringView(tagBuffer.data());
    if (newTagValue.empty())
    {
      std::copy_n(currentTag.begin(), lengthToCopy, tagBuffer.data());
      tagBuffer[currentTag.size()] = '\0';
    }
    else
    {
      tagComponent.UpdateValue(newTagValue);
    }
  }
  if (ImGui::IsItemHovered())
    ImGui::SetTooltip("Press ENTER to confirm changes");
}
void InspectorPanel::__RenderTransformComponent(Transform& transform) const
{
  if (ImGui::BeginTable("Transformation_Table", 3, ImGuiTableFlags_SizingFixedFit))
  {
    ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthFixed, 80.f);
    ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthFixed, 24.f);

    ImGui::TableNextRow();
    __RenderTransformTableRow("Position", transform.position, Vec3F(0.f));

    ImGui::TableNextRow();
    __RenderTransformTableRow("Rotation", transform.eulerAngles, Vec3F(0.f));

    ImGui::TableNextRow();
    __RenderTransformTableRow("Scale", transform.scale, Vec3F(1.f));

    ImGui::EndTable();
  }
}
void InspectorPanel::__RenderTransformTableRow(StringView label, Vec3F& values, Vec3F resetValues) const
{
  ImGui::PushID(label.data());

  // Column 1: Label
  ImGui::TableNextColumn();
  ImGui::Text(label.data());
  // Column 2: drag float input
  ImGui::TableNextColumn();
  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

  auto lineHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
  auto buttonSize = ImVec2{ lineHeight + 3.0f, lineHeight };
  auto itemWidth = ImGui::CalcItemWidth() / 3.0f;

  // X value
  ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
  ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.7f, 0.05f, 0.1f, 1.0f });
  ImGui::Button("X", buttonSize);
  ImGui::PopStyleColor(3);
  ImGui::SameLine();
  ImGui::SetNextItemWidth(itemWidth);
  ImGui::DragFloat("##X", &values.x, 0.1f);
  ImGui::SameLine();

  // Y value
  ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
  ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.6f, 0.1f, 1.0f });
  ImGui::Button("Y", buttonSize);
  ImGui::PopStyleColor(3);
  ImGui::SameLine();
  ImGui::SetNextItemWidth(itemWidth);
  ImGui::DragFloat("##Y", &values.y, 0.1f);
  ImGui::SameLine();

  // Z value
  ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
  ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.05f, 0.2f, 0.7f, 1.0f });
  ImGui::Button("Z", buttonSize);
  ImGui::PopStyleColor(3);
  ImGui::SameLine();
  ImGui::SetNextItemWidth(itemWidth);
  ImGui::DragFloat("##Z", &values.z, 0.1f);
  ImGui::PopStyleVar();
  
  // Column 3: reset icon
  ImGui::TableNextColumn();

  auto isModified = values != resetValues;
  if (isModified)
  {
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.f, 0.f, 0.f, 0.f });
    if (ImGui::ImageButton("##ResetButton", _resetIcon.id, ImVec2{ 16, 16 }))
      values = resetValues;
    ImGui::PopStyleColor();

    if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
      ImGui::SetTooltip("Reset %s", label.data());
  }
  
  ImGui::PopID();
}
void InspectorPanel::__RenderCameraComponent(Camera& camera) const
{
  if (ImGui::BeginTable("Camera_Table", 3, ImGuiTableFlags_SizingFixedFit))
  {
    ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthFixed, 80.f);
    ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthFixed, 24.f);

    // Position
    ImGui::TableNextRow();
    __RenderTransformTableRow("Position", camera.position, Vec3F(0.f));

    // Rotation
    ImGui::TableNextRow();
    __RenderTransformTableRow("Rotation", camera.eulerAngles, Vec3F(0.f));

    // Horizontal FOV
    ImGui::TableNextRow();
    __RenderProjectionRow("FovH", camera, 60.f); // reset value: 60.0f degrees

    // Near Clip
    ImGui::TableNextRow();
    __RenderProjectionRow("Near Clip", camera, 0.1f);

    // Far Clip
    ImGui::TableNextRow();
    __RenderProjectionRow("Far Clip", camera, 50.0f);

    ImGui::EndTable();
  }
}
void InspectorPanel::__RenderProjectionRow(StringView label, Camera& camera, f32 resetValue) const
{
  ImGui::PushID(label.data());

  // Column 1: Label
  ImGui::TableNextColumn();
  ImGui::Text(label.data());

  // Column 2: Input Field
  ImGui::TableNextColumn();

  auto totalAvailableWidth = ImGui::GetContentRegionAvail();

  auto dragSpeed = 0.5f;
  auto currentValue = 0.0f;
  if (label.compare("FovH") == 0)
  {
    currentValue = camera.fovH; // in degrees
    ImGui::SetNextItemWidth(totalAvailableWidth.x);
    if (ImGui::DragFloat("##value", &currentValue, dragSpeed, 0.0f, 180.0f, "%.2f (° degrees)", ImGuiSliderFlags_ClampOnInput))
      camera.fovH = currentValue;
  }
  else if (label.compare("Near Clip") == 0)
  {
    currentValue = camera.nearClip;
    ImGui::SetNextItemWidth(totalAvailableWidth.x);
    if (ImGui::DragFloat("##value", &currentValue, dragSpeed, 0.1f, 10.0f, "%.2f", ImGuiSliderFlags_ClampOnInput))
      camera.nearClip = currentValue;
  }
  else if (label.compare("Far Clip") == 0)
  {
    currentValue = camera.farClip;
    ImGui::SetNextItemWidth(totalAvailableWidth.x);
    if (ImGui::DragFloat("##value", &currentValue, dragSpeed, camera.nearClip, 100.0f, "%.2f", ImGuiSliderFlags_ClampOnInput))
      camera.farClip = currentValue;
  }

  // Column 3: Reset Button
  ImGui::TableNextColumn();
  auto isModified = currentValue != resetValue;
  if (isModified)
  {
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.f, 0.f, 0.f, 0.f });
    if (ImGui::ImageButton("##ResetButton", _resetIcon.id, ImVec2{ 16, 16 }))
    {
      if (label.compare("FovH") == 0)
        camera.fovH = resetValue;
      else if (label.compare("Near Clip") == 0)
        camera.nearClip = resetValue;
      else if (label.compare("Far Clip") == 0)
        camera.farClip = resetValue;
    }
    ImGui::PopStyleColor();
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal) && isModified)
      ImGui::SetTooltip("Reset %s", label.data());
  }
  ImGui::PopID();
}

//void InspectorPanel::__ListAllComponents(Entity entity)
//{
//  if (ImGui::CollapsingHeader("Tag"))
//  {
//    auto tag = entity.GetComponent<Tag>();
//    Insp_Tag(*tag);
//  }
//  if (auto transform = entity.GetComponent<Transform>())
//  {
//    if (ImGui::CollapsingHeader("Transform"))
//      Insp_Transformation(entity, *transform);
//  }
//  if (auto staticMesh = entity.GetComponent<StaticMesh>())
//  {
//    if (ImGui::CollapsingHeader("StaticMesh"))
//      Insp_StaticMesh(entity, *staticMesh);
//  }
//  if (auto light = entity.GetComponent<Light>())
//  {
//    if (ImGui::CollapsingHeader("Light"))
//    {
//      switch (light->type)
//      {
//        case LightType::Directional:
//        {
//          auto dirLight = entity.GetComponent<DirectionalLight>();
//          Insp_DirectLight(entity, *dirLight);
//          break;
//        }
//        case LightType::Point:
//        {
//          auto pointLight = entity.GetComponent<PointLight>();
//          Insp_PointLight(entity, *pointLight);
//          break;
//        }
//        case LightType::Spot:
//        {
//          auto spotLight = entity.GetComponent<SpotLight>();
//          Insp_SpoLight(entity, *spotLight);
//          break;
//        }
//      }
//    }
//  }
//}

//bool InspectorPanel::__ButtonCentered(StringView label, Vec2I size)
//{
//  auto avail = ImGui::GetContentRegionAvail().x;
//  auto off = (avail - size.x) * 0.5f;
//  if (off > 0.0f)
//    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);
//
//  return ImGui::Button(label.data(), ImVec2(size.x, size.y));
//}

//void InspectorPanel::__NewComponentPopup(Entity entity)
//{
//  if (ImGui::BeginPopup("NewComponent_Popup"))
//  {
//    // Add Transform component
//    // --------------------------------
//    __AddTransformComponent(entity);
//
//    ImGui::Spacing();
//
//    // Add Light component
//    // --------------------------------
//    __AddLightComponent(entity);
//
//    ImGui::Spacing();
//
//    // Add StaticMesh component
//    // --------------------------------
//    __AddStaticMeshComponent(entity);
//
//    ImGui::EndPopup();
//  }
//}

#if 0 
#include "Core/Paths/Paths.hpp"
#include "Core/Dialog/FileDialog.hpp"
#include "Engine/Managers/TexturesManager.hpp"
#include "Engine/Managers/StaticMeshFactory.hpp"
#include <ImGuizmo.h>

static void Insp_Light_ComboAttenuation(f32& kl, f32& kq)
{
  static auto buffer = Array<char, 64>{};
  buffer.fill(0);

  // Find current selection using std::find_if with lambda
  auto currentIt = std::find_if(ATTENUATION_RANGES.begin(), 
                                ATTENUATION_RANGES.end(), 
                                [kl, kq](const Array<f32, 3>& data) 
  {
    return std::abs(data[1] - kl) < 0.001f && std::abs(data[2] - kq) < 0.001f;
  });

  auto currentSelection = (currentIt != ATTENUATION_RANGES.end()) ?
    std::distance(ATTENUATION_RANGES.begin(), currentIt) : -1;

  // Create concise preview string
  if (currentSelection >= 0)
  {
    std::format_to_n(buffer.data(), buffer.size(), "Range %d", static_cast<i32>((*currentIt)[0]));
  }
  else
  {
    constexpr auto text = StringView("Custom");
    constexpr auto len = text.length();
    std::copy_n(text.begin(), len, buffer.begin());
  }


  if (ImGui::BeginCombo("##AttenuationCombo", buffer.data()))
  {
    // Add "Custom" option at the top
    auto isCustomSelected = (currentSelection == -1);
    if (ImGui::Selectable("Custom", isCustomSelected))
    {
      // Don't change values, just close combo
    }

    // Add separator
    ImGui::Separator();

    // Add preset options using enumerate pattern
    auto index = 0;
    std::for_each(ATTENUATION_RANGES.begin(), 
                  ATTENUATION_RANGES.end(),
                  [&](const Array<f32, 3>& data) 
    {
      
      std::format_to_n(buffer.data(),
                       buffer.size(), 
                       "Range %d (kl: %.3f, kq: %.4f)", 
                       static_cast<int>(data[0]), data[1], data[2]);

      auto isSelected = (currentSelection == index);
      if (ImGui::Selectable(buffer.data(), isSelected))
      {
        kl = data[1];
        kq = data[2];
      }
      if (isSelected)
        ImGui::SetItemDefaultFocus();
      
      ++index;
    });

    ImGui::EndCombo();
  }

  // Show current values as text below the combo
  ImGui::SameLine();
  if (ImGui::Button("Edit"))
  {
    ImGui::OpenPopup("EditAttenuation");
  }
  if (ImGui::BeginPopup("EditAttenuation"))
  {
    ImGui::Text("Manual Attenuation Values");
    ImGui::Separator();

    ImGui::DragFloat("Linear (kl)", &kl, 0.001f, 0.0f, 2.0f, "%.4f");
    ImGui::DragFloat("Quadratic (kq)", &kq, 0.001f, 0.0f, 2.0f, "%.6f");

    if (ImGui::Button("Close"))
    {
      ImGui::CloseCurrentPopup();
    }

    ImGui::EndPopup();
  }
}
static void Insp_DirectLight(Entity entity, DirectionalLight& light)
{
  // Create a table with two columns: one for the labels and one for input
  if (ImGui::BeginTable("Light_Table", 2, ImGuiTableFlags_SizingFixedFit))
  {
    ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthFixed, 80.f);
    ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthStretch);

    // Row 1: view light type
    ImGui::TableNextRow();
    ImGui::TableNextColumn(); // Column 1: label
    ImGui::Text("Type");
    ImGui::TableNextColumn(); // Second column: combo
    ImGui::BeginDisabled();
    if (ImGui::BeginCombo("##Type", "Directional"))
      ImGui::EndCombo();
    ImGui::EndDisabled();

    // Row 2: color input
    ImGui::TableNextRow();
    ImGui::TableNextColumn(); // Column 1: label
    ImGui::Text("Color");
    ImGui::TableNextColumn(); // Column 2: input
    ImGui::ColorEdit3("##Color", reinterpret_cast<f32*>(&light.color));

    // Row 3: intensity input
    ImGui::TableNextRow();
    ImGui::TableNextColumn(); // Column 1: label
    ImGui::Text("Intensity");
    ImGui::TableNextColumn(); // Column 2: input
    ImGui::SliderFloat("##Intensity", &light.intensity, 0.0f, 1.0f);

    // Row 4: direction input
    ImGui::TableNextRow();
    ImGui::TableNextColumn(); // Column 1: label
    ImGui::Text("Direction");
    ImGui::TableNextColumn(); // Column 2: input
    ImGui::DragFloat3("##Direction", reinterpret_cast<f32*>(&light.direction), 0.1f, -FLT_MAX, FLT_MAX);

    ImGui::EndTable();
  }
}
static void Insp_PointLight(Entity entity, PointLight& light)
{
  // Create a table with two columns: one for the labels and one for input
  if (ImGui::BeginTable("Light_Table", 2, ImGuiTableFlags_SizingFixedFit))
  {
    ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthFixed, 80.f);
    ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthStretch);

    // Row 1: view light type
    ImGui::TableNextRow();
    ImGui::TableNextColumn(); // Column 1: label
    ImGui::Text("Type");
    ImGui::TableNextColumn(); // Column 2: combo
    ImGui::BeginDisabled();
    if (ImGui::BeginCombo("##Type", "Point"))
      ImGui::EndCombo();
    ImGui::EndDisabled();

    // Row 2: color input
    ImGui::TableNextRow();
    ImGui::TableNextColumn(); // Column 1: label
    ImGui::Text("Color");
    ImGui::TableNextColumn(); // Column 2: input
    ImGui::ColorEdit3("##Color", reinterpret_cast<f32 *>(&light.color));

    // Row 3: diffuse input
    ImGui::TableNextRow();
    ImGui::TableNextColumn(); // Column 1: label
    ImGui::Text("Intensity");
    ImGui::TableNextColumn(); // Column 2: input
    ImGui::SliderFloat("##Intensity", &light.intensity, 0.0f, 1.0f);

    // Row 4: position input
    ImGui::TableNextRow();
    ImGui::TableNextColumn(); // Column 1: label
    ImGui::Text("Position");
    ImGui::TableNextColumn(); // Column 2: input
    ImGui::DragFloat3("##Position", reinterpret_cast<f32 *>(&light.position), 0.1f, -FLT_MAX, FLT_MAX);

    // Row 4: attenuation combo
    ImGui::TableNextRow();
    ImGui::TableNextColumn(); // Column 1: label
    ImGui::TextWrapped("Attenuation");
    ImGui::TableNextColumn(); // Column 2: input
    Insp_Light_ComboAttenuation(light.kl, light.kq);
    ImGui::EndTable();
  }
}
static void Insp_SpoLight(Entity entity, SpotLight& light)
{
  // Crea una tabella con due colonne per le etichette e gli input
  if (ImGui::BeginTable("Spotlight_Table", 2, ImGuiTableFlags_SizingFixedFit))
  {
    ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthFixed, 80.f);
    ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthStretch);

    // Riga 1: tipo di luce (fisso su "Spot")
    ImGui::TableNextRow();
    ImGui::TableNextColumn(); // Colonna 1: etichetta
    ImGui::Text("Type");
    ImGui::TableNextColumn(); // Colonna 2: combo disabilitato
    ImGui::BeginDisabled();
    if (ImGui::BeginCombo("##Type", "Spot"))
      ImGui::EndCombo();
    ImGui::EndDisabled();

    // Riga 2: input colore
    ImGui::TableNextRow();
    ImGui::TableNextColumn(); // Colonna 1: etichetta
    ImGui::Text("Color");
    ImGui::TableNextColumn(); // Colonna 2: input
    ImGui::ColorEdit3("##Color", reinterpret_cast<f32*>(&light.color));

    // Riga 3: input intensità
    ImGui::TableNextRow();
    ImGui::TableNextColumn(); // Colonna 1: etichetta
    ImGui::Text("Intensity");
    ImGui::TableNextColumn(); // Colonna 2: input
    ImGui::SliderFloat("##Intensity", &light.intensity, 0.0f, 1.0f);

    // Riga 4: input posizione
    ImGui::TableNextRow();
    ImGui::TableNextColumn(); // Colonna 1: etichetta
    ImGui::Text("Position");
    ImGui::TableNextColumn(); // Colonna 2: input
    ImGui::DragFloat3("##Position", reinterpret_cast<f32*>(&light.position), 0.1f, -FLT_MAX, FLT_MAX);

    // Riga 5: input direzione
    ImGui::TableNextRow();
    ImGui::TableNextColumn(); // Colonna 1: etichetta
    ImGui::Text("Direction");
    ImGui::TableNextColumn(); // Colonna 2: input
    ImGui::DragFloat3("##Direction", reinterpret_cast<f32*>(&light.direction), 0.01f, -1.0f, 1.0f);

    // Riga 6: attenuazione (con un combo personalizzato o slider)
    ImGui::TableNextRow();
    ImGui::TableNextColumn(); // Colonna 1: etichetta
    ImGui::TextWrapped("Attenuation");
    ImGui::TableNextColumn(); // Colonna 2: input
    // Insp_Light_ComboAttenuation(light.kl, light.kq); // Se vuoi usare un combo
    ImGui::DragFloat("##Linear", &light.kl, 0.001f, 0.0f, 1.0f, "Linear");
    ImGui::DragFloat("##Quadratic", &light.kq, 0.001f, 0.0f, 1.0f, "Quadratic");

    // Riga 7: angoli del cono
    ImGui::TableNextRow();
    ImGui::TableNextColumn(); // Colonna 1: etichetta
    ImGui::TextWrapped("Cone Angles");
    ImGui::TableNextColumn(); // Colonna 2: input
    ImGui::SliderFloat("##Outer Angle", &light.thetaU, 0.0f, 90.0f, "Outer Angle (%.1f deg)");
    ImGui::SliderFloat("##Inner Angle", &light.thetaP, 0.0f, 90.0f, "Inner Angle (%.1f deg)");

    ImGui::EndTable();
  }
}



static void Insp_ShowTextureSelector(StringView label, 
                                     u32 meshID, 
                                     Texture2D& meshTexture, 
                                     Texture2D defaultTex)
{
  auto& texManager = TexturesManager::GetInstance();
  auto isMeshTextureValid = meshTexture.GetWidth() != 1;

  // Column 1: label
  ImGui::TableNextColumn();
  ImGui::Text(label.data());

  // Column 2: combo
  ImGui::TableNextColumn();
  ImGui::SetNextItemWidth(ImGui::GetColumnWidth());
  if (isMeshTextureValid)
  {
    auto texturePath = texManager.GetTexturePath(meshTexture.id);
    ImGui::TextWrapped(texturePath->string().c_str());
  }
  else
    ImGui::TextWrapped("No texture");

  // Column 3: reset button
  ImGui::TableNextColumn();
  if (isMeshTextureValid)
  {
    static auto resetIcon = texManager.GetOrCreateIcon("reset-arrow-16.png");

    auto buttonID = Array<char, 64>{}; // e.g. "Reset##Mesh_1_Diffuse"
    std::format_to(buttonID.begin(), "Reset##Mesh_{}_{}", meshID, label.data());

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.f, 0.f, 0.f, 0.f});
    if (ImGui::ImageButton(buttonID.data(), resetIcon.id, ImVec2(16.f, 16.f)))
      meshTexture = defaultTex;
    ImGui::PopStyleColor();
  }


  // "Choose texture" button
  ImGui::TableNextRow();
  ImGui::TableSetColumnIndex(1); // Column 2
  {
    auto buttonID = Array<char, 64>{}; // e.g. "Reset##Mesh_1_Diffuse"
    std::format_to(buttonID.begin(), "Choose texture##Mesh_{}_{}", meshID, label.data());
    if (ImGui::Button(buttonID.data()))
    {
      const char* filterPatterns[] = { "*.png", "*.jpg" };
      constexpr auto nrFilterPatterns = sizeof(filterPatterns) / sizeof(filterPatterns[0]);
      auto path = FileDialog::OpenFileDialog("Choose a texture for the 3D mesh",
                                             Paths::GetTexturesPath(),
                                             nrFilterPatterns,
                                             filterPatterns,
                                             "Image files (*.png, *jpg)",
                                             false);
      if (!path.empty())
      {
        path = fs::relative(path, Paths::GetTexturesPath());
        meshTexture = texManager.GetOrCreateTexture(path);
      }
    }
  }
}
static void Insp_StaticMesh(Entity entity, StaticMesh& staticMesh)
{
  auto& texManager = TexturesManager::GetInstance();
  
  ImGui::Text("Nr meshes: %d", staticMesh.nrMeshes);
  // View meshes with a tree
  if (ImGui::TreeNode("Material"))
  {
    for (auto i = 0u; i < staticMesh.nrMeshes; i++)
    {
      auto& mesh = staticMesh.meshArray[i];
      auto& material = mesh.material;

      auto meshLabel = Array<char, 32>{};
      std::format_to(meshLabel.begin(), "Mesh_{}", i + 1);
      if (ImGui::TreeNode(meshLabel.data()))
      {
        if (ImGui::BeginTable("TextureTable", 3, ImGuiTableFlags_SizingFixedFit))
        {
          ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthFixed, 48.0f);
          ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthStretch);
          ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthFixed, 24.0f);

          // Diffuse row
          ImGui::TableNextRow();
          Insp_ShowTextureSelector("Diffuse", i, material.albedo, texManager.GetDefaultDiffuse());

          // Specular row
          //ImGui::TableNextRow();
          //Insp_ShowTextureSelector("Specular", i, material.specular, texManager.GetDefaultSpecular());

          // Normal row
          ImGui::TableNextRow();
          Insp_ShowTextureSelector("Normal", i, material.normalMap, texManager.GetDefaultNormal());

          ImGui::EndTable();
        }
        ImGui::TreePop();
      }
    }

    ImGui::TreePop();
  }
}
#endif
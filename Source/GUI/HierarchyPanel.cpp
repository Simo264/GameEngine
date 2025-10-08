#include "HierarchyPanel.hpp"

#include "Utils/Logger.hpp"
#include "Utils/FileDialog.hpp"
#include "Engine/Paths.hpp"
#include "Engine/Managers/TexturesManager.hpp"
#include "Engine/ECS/Scene.hpp"
#include "Engine/ECS/Builders/StaticMeshEntityBuilder.hpp"
#include "Engine/ECS/Builders/LightSourceEntityBuilder.hpp"

#include <imgui.h>

using namespace Components;

struct EntityCreationData
{
  Tag::TagValue tag{ "entity" };
  bool tagIsValid{ false };

  const Archetype* selectedArchetype{ nullptr };
  ArchetypeId selectedArchetypeId{ INVALID_ARCHETYPE_ID };

  LightType lightType{ LightType::None };
  fs::path modelPath{};
};
auto s_entityData = EntityCreationData{};


void HierarchyPanel::Render()
{
  if (!isOpen || !_scene)
    return;

  ImGui::Begin("Hierarchy", &isOpen);

  auto btnWidth = ImGui::GetContentRegionAvail().x - 32.f;
  if (__ButtonCentered("+New entity", Vec2I(btnWidth, 26.f)))
  {
    s_entityData = EntityCreationData{};
    ImGui::OpenPopup("New entity modal");
  }
  if (ImGui::BeginPopupModal("New entity modal", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
  {
    __RenderNewEntityModal();
    ImGui::EndPopup();
  }

  ImGui::Spacing();
  ImGui::Separator();
  ImGui::Spacing();

  __RenderEntityList();

  ImGui::End();
}

bool HierarchyPanel::__ButtonCentered(StringView label, Vec2I size)
{
  auto avail = ImGui::GetContentRegionAvail().x;
  auto off = (avail - size.x) * 0.5f;
  if (off > 0.0f)
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);

  return ImGui::Button(label.data(), ImVec2(size.x, size.y));
}
void HierarchyPanel::__RenderNewEntityModal()
{
  // Edit Tag
  ImGui::BeginGroup();
  ImGui::Text("Edit tag");
  ImGui::InputText("##Tag", s_entityData.tag.data(), s_entityData.tag.size());
  s_entityData.tagIsValid = strlen(s_entityData.tag.data()) > 0;
  if (!s_entityData.tagIsValid)
  {
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Cannot be empty!");
  }
  ImGui::EndGroup();

  ImGui::Spacing();
  ImGui::Separator();
  ImGui::Spacing();

  // Archetype selection
  ImGui::BeginGroup();
  ImGui::Text("Archetype Selection");
  
  // Show all available archetypes
  auto previousSelection = s_entityData.selectedArchetypeId;
  auto& archetypeRegistry = _scene->GetArchetypeRegistry();
  auto& availableArchetypes = archetypeRegistry.GetArchetypeVector();
  for (ArchetypeId i = 0; i < availableArchetypes.size(); i++)
  {
    auto& archetype = archetypeRegistry.GetArchetype(i);

    auto isSelected = (s_entityData.selectedArchetypeId == i);
    if (ImGui::RadioButton(archetype.GetName(), isSelected))
    {
      s_entityData.selectedArchetype = &archetype;
      s_entityData.selectedArchetypeId = i;
    }
    ImGui::SameLine();
  }
  ImGui::EndGroup();

  if (s_entityData.selectedArchetypeId != INVALID_ARCHETYPE_ID)
  {
    auto archetypeName = StringView(s_entityData.selectedArchetype->GetName());
    if (archetypeName.compare("LightSource") == 0)
    {
      ImGui::Spacing();
      ImGui::Separator();
      ImGui::Spacing();
      ImGui::BeginGroup();
      ImGui::Text("Light Type");
      ImGui::RadioButton("Directional", reinterpret_cast<i32*>(&s_entityData.lightType), static_cast<i32>(LightType::Directional));
      ImGui::SameLine();
      ImGui::RadioButton("Point", reinterpret_cast<i32*>(&s_entityData.lightType), static_cast<i32>(LightType::Point));
      ImGui::SameLine();
      ImGui::RadioButton("Spot", reinterpret_cast<i32*>(&s_entityData.lightType), static_cast<i32>(LightType::Spot));
      ImGui::EndGroup();
    }
    else if (archetypeName.compare("StaticMesh") == 0)
    {
      ImGui::Spacing();
      ImGui::Separator();
      ImGui::Spacing();
      ImGui::BeginGroup();
      ImGui::Text("Mesh File");
      ImGui::Text("%s", s_entityData.modelPath.empty() ? "No file selected." : s_entityData.modelPath.string().c_str());
      ImGui::SameLine();
      if (ImGui::Button("Choose model"))
      {
        const char* filterPatterns[] = { "*.obj", "*.glb", "*.gltf", "*.fbx" };
        constexpr auto nrFilterPatterns = sizeof(filterPatterns) / sizeof(filterPatterns[0]);
        s_entityData.modelPath = FileDialog::OpenFileDialog("Choose 3D model",
                                                            GetModelsPath(),
                                                            nrFilterPatterns,
                                                            filterPatterns,
                                                            "3D model file (*.obj, *.glb, *.gltf, *.fbx)",
                                                            false);
      }
      ImGui::EndGroup();
    }
    else if (archetypeName.compare("Camera") == 0)
    {
      ImGui::Text("TODO CAMERA SECTION");
    }
  }

  ImGui::Spacing();
  ImGui::Separator();
  ImGui::Spacing();

  auto canCreate = s_entityData.tagIsValid &&
    s_entityData.selectedArchetype != nullptr &&
    s_entityData.selectedArchetypeId != INVALID_ARCHETYPE_ID &&
    (
      (StringView(s_entityData.selectedArchetype->GetName()).compare("LightSource") == 0 && s_entityData.lightType != LightType::None)
    ||
      (StringView(s_entityData.selectedArchetype->GetName()).compare("StaticMesh") == 0) && !s_entityData.modelPath.empty()
    );

  if (canCreate)
  {
    if (ImGui::Button("Create"))
    {
      auto archetypeName = StringView(s_entityData.selectedArchetype->GetName());
      auto& archetypeId = s_entityData.selectedArchetypeId;
      auto& tag = s_entityData.tag;

      if (archetypeName.compare("StaticMesh") == 0)
      {
        auto& modelPath = s_entityData.modelPath;
        auto builder = StaticMeshEntityBuilder{ *_scene };
        builder.WithArchetypeIdentifier(archetypeId);
        builder.WithTag(tag.data());
        builder.WithModelPath(modelPath);
        auto entity = builder.Build();
      }
      else if (archetypeName.compare("LightSource") == 0)
      {
        auto builder = LightSourceEntityBuilder{ *_scene };
        builder.WithArchetypeIdentifier(archetypeId);
        builder.WithTag(tag.data());
        builder.WithLightType(s_entityData.lightType);
        auto entity = builder.Build();
      }
      
      ImGui::CloseCurrentPopup();
    }
  }
  else
  {
    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.5f);
    ImGui::Button("Create");
    ImGui::PopStyleVar();
  }

  ImGui::SameLine();

  if (ImGui::Button("Cancel"))
    ImGui::CloseCurrentPopup();
}
void HierarchyPanel::__RenderEntityList()
{
  auto& texManager = TexturesManager::GetInstance();
  static auto icon = texManager.GetOrCreateIcon("game-object-16.png");

  auto selectableName = Array<char, 64>{};

  auto& entityRegistry = _scene->GetEntityRegistry();
  auto& archetypeRegistry = _scene->GetArchetypeRegistry();
  for (auto e : entityRegistry.view<ArchetypeIdentifier>())
  {
    auto entity = Entity{ e };
    auto& archetypeComponent =  _scene->GetEntityComponent<ArchetypeIdentifier>(entity);
    auto archetypeId = archetypeComponent.archetypeId;
    auto& archetype = archetypeRegistry.GetArchetype(archetypeId);
    
    auto& tagComponent = _scene->GetEntityComponent<Tag>(entity);
    selectableName.fill(0);
    std::format_to_n(selectableName.data(),
                     selectableName.size(),
                     "{}##{}",
                     tagComponent.value.data(),
                     static_cast<u32>(entity.Id()));

    ImGui::BeginGroup();
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + (ImGui::GetTextLineHeight() - 16.f) / 2);

    ImGui::Image(icon.id, ImVec2(16.f, 16.f));
    ImGui::SameLine();

    auto& colors = ImGui::GetStyle().Colors;
    auto colorSelected = colors[ImGuiCol_FrameBgActive];
    auto colorHovered = colors[ImGuiCol_FrameBgHovered];
    ImGui::PushStyleColor(ImGuiCol_Header, colorSelected);
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, colorHovered);
    ImGui::PushStyleColor(ImGuiCol_HeaderActive, colorSelected);

    auto isSelected = ImGui::Selectable(selectableName.data(), target == entity);
    if (isSelected && target != entity)
      target = entity;

    if (ImGui::IsItemHovered() &&
        ImGui::IsMouseClicked(ImGuiMouseButton_Right) &&
        target.Valid())
      ImGui::OpenPopup("Entity Popup");

    ImGui::PopStyleColor(3);
    ImGui::EndGroup();

    ImGui::Spacing();
  }

  if (ImGui::BeginPopup("Entity Popup")) // Display menu on right click entity
  {
    if (ImGui::MenuItem("Delete entity"))
    {
      _scene->DestroyEntity(target.Id());
      target = Entity{};
    }
    ImGui::EndPopup();
  }
}

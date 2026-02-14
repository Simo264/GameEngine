#include "hierarchy_panel.hpp"

#include "utils/logger.hpp"
#include "utils/file_dialog.hpp"
#include "engine/paths.hpp"
#include "engine/managers/textures_manager.hpp"
#include "engine/ecs/scene.hpp"
#include "engine/ecs/builders/static_mesh_entity_builder.hpp"
#include "engine/ecs/builders/light_source_entity_builder.hpp"

#include <imgui.h>

#include <array>
#include <filesystem>

using fs = std::filesystem;

using namespace Components;

struct EntityCreationData
{
  std::array<char, Tag::max_len> tag{ "entity" };
  bool is_tag_valid{ false };

  const Archetype* selected_archetype{ nullptr };
  ArchetypeId selected_archetype_id{ INVALID_ARCHETYPE_ID };

  LightType light_type{ LightType::None };
  fs::path model_path{};
};
auto s_entity_data = EntityCreationData{};


void HierarchyPanel::render()
{
  if (!is_open || !scene_)
    return;

  ImGui::Begin("Hierarchy", &is_open);

  auto btn_width = ImGui::GetContentRegionAvail().x - 32.f;
  if (button_centered("+New entity", Vec2I(btn_width, 26.f)))
  {
    s_entity_data = EntityCreationData{};
    ImGui::OpenPopup("New entity modal");
  }
  if (ImGui::BeginPopupModal("New entity modal", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
  {
    render_new_entity_modal();
    ImGui::EndPopup();
  }

  ImGui::Spacing();
  ImGui::Separator();
  ImGui::Spacing();

  render_entity_list();

  ImGui::End();
}

bool HierarchyPanel::button_centered(std::string_view label, f32 width, f32 height)
{
  auto avail = ImGui::GetContentRegionAvail().x;
  auto off = (avail - width) * 0.5f;
  if (off > 0.0f)
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);

  return ImGui::Button(label.data(), ImVec2(width, height));
}
void HierarchyPanel::render_new_entity_modal()
{
  // Edit Tag
  ImGui::BeginGroup();
  ImGui::Text("Edit tag");
  ImGui::InputText("##Tag", s_entity_data.tag.data(), s_entity_data.tag.size());
  s_entity_data.is_tag_valid = strlen(s_entity_data.tag.data()) > 0;
  if (!s_entity_data.is_tag_valid)
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
  auto previous_selection = s_entity_data.selected_archetype_id;
  auto& archetype_registry = _scene->GetArchetypeRegistry();
  auto& available_archetypes = archetype_registry.GetArchetypeVector();
  for (ArchetypeId i = 0; i < available_archetypes.size(); i++)
  {
    auto& archetype = archetype_registry.GetArchetype(i);

    auto is_selected = (s_entity_data.selected_archetype_id == i);
    if (ImGui::RadioButton(archetype.GetName(), is_selected))
    {
      s_entity_data.selected_archetype = &archetype;
      s_entity_data.selected_archetype_id = i;
    }
    ImGui::SameLine();
  }
  ImGui::EndGroup();

  if (s_entity_data.selected_archetype_id != INVALID_ARCHETYPE_ID)
  {
    auto archetype_name = std::string_view(s_entity_data.selected_archetype->GetName());
    if (archetype_name.compare("LightSource") == 0)
    {
      ImGui::Spacing();
      ImGui::Separator();
      ImGui::Spacing();
      ImGui::BeginGroup();
      ImGui::Text("Light Type");
      ImGui::RadioButton("Directional", reinterpret_cast<i32*>(&s_entity_data.light_type), static_cast<i32>(LightType::Directional));
      ImGui::SameLine();
      ImGui::RadioButton("Point", reinterpret_cast<i32*>(&s_entity_data.light_type), static_cast<i32>(LightType::Point));
      ImGui::SameLine();
      ImGui::RadioButton("Spot", reinterpret_cast<i32*>(&s_entity_data.light_type), static_cast<i32>(LightType::Spot));
      ImGui::EndGroup();
    }
    else if (archetype_name.compare("StaticMesh") == 0)
    {
      ImGui::Spacing();
      ImGui::Separator();
      ImGui::Spacing();
      ImGui::BeginGroup();
      ImGui::Text("Mesh File");
      ImGui::Text("%s", s_entity_data.model_path.empty() ? "No file selected." : s_entity_data.model_path.string().c_str());
      ImGui::SameLine();
      if (ImGui::Button("Choose model"))
      {
        const char* filter_patterns[] = { "*.obj", "*.glb", "*.gltf", "*.fbx" };
        constexpr auto nr_filter_patterns = sizeof(filter_patterns) / sizeof(filter_patterns[0]);
        s_entity_data.model_path = open_file_dialog("Choose 3D model",
                                                  GetModelsPath(),
                                                  nr_filter_patterns,
                                                  filter_patterns,
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

  auto canCreate = s_entityData.is_tag_valid &&
    s_entityData.selected_archetype != nullptr &&
    s_entityData.selected_archetype_id != INVALID_ARCHETYPE_ID &&
    (
      (StringView(s_entityData.selected_archetype->GetName()).compare("LightSource") == 0 && s_entityData.light_type != LightType::None)
    ||
      (StringView(s_entityData.selected_archetype->GetName()).compare("StaticMesh") == 0) && !s_entityData.model_path.empty()
    );

  if (canCreate)
  {
    if (ImGui::Button("Create"))
    {
      auto archetype_name = StringView(s_entityData.selected_archetype->GetName());
      auto& archetype_id = s_entityData.selected_archetype_id;
      auto& tag = s_entityData.tag;

      if (archetype_name.compare("StaticMesh") == 0)
      {
        auto& model_path = s_entityData.model_path;
        auto builder = StaticMeshEntityBuilder{ *_scene };
        builder.WithArchetypeIdentifier(archetype_id);
        builder.WithTag(tag.data());
        builder.WithModelPath(model_path);
        auto entity = builder.Build();
      }
      else if (archetype_name.compare("LightSource") == 0)
      {
        auto builder = LightSourceEntityBuilder{ *_scene };
        builder.WithArchetypeIdentifier(archetype_id);
        builder.WithTag(tag.data());
        builder.WithLightType(s_entityData.light_type);
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
void HierarchyPanel::render_entity_list()
{
  auto& tex_manager = TexturesManager::instance();
  static auto icon = tex_manager.GetOrCreateIcon("game-object-16.png");

  auto selectable_name = Array<char, 64>{};

  auto& entity_registry = _scene->GetEntityRegistry();
  auto& archetype_registry = _scene->GetArchetypeRegistry();
  for (auto e : entity_registry.view<ArchetypeIdentifier>())
  {
    auto entity = Entity{ e };
    auto& archetype_component =  _scene->GetEntityComponent<ArchetypeIdentifier>(entity);
    auto archetype_id = archetype_component.archetypeId;
    auto& archetype = archetype_registry.GetArchetype(archetype_id);
    
    auto& tag_component = _scene->GetEntityComponent<Tag>(entity);
    selectable_name.fill(0);
    std::format_to_n(selectable_name.data(),
                     selectable_name.size(),
                     "{}##{}",
                     tag_component.value.data(),
                     static_cast<u32>(entity.Id()));

    ImGui::BeginGroup();
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + (ImGui::GetTextLineHeight() - 16.f) / 2);

    ImGui::Image(icon.id, ImVec2(16.f, 16.f));
    ImGui::SameLine();

    auto& colors = ImGui::GetStyle().Colors;
    auto color_selected = colors[ImGuiCol_FrameBgActive];
    auto color_hovered = colors[ImGuiCol_FrameBgHovered];
    ImGui::PushStyleColor(ImGuiCol_Header, color_selected);
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, color_hovered);
    ImGui::PushStyleColor(ImGuiCol_HeaderActive, color_selected);

    auto is_selected = ImGui::Selectable(selectable_name.data(), target == entity);
    if (is_selected && target != entity)
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
      scene_->DestroyEntity(target.id());
      target = Entity{};
    }
    ImGui::EndPopup();
  }
}

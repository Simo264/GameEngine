#include "inspector_panel.hpp"
#include "engine/ecs/scene.hpp"
#include "engine/managers/texturesmanager.hpp"
#include <imgui.h>

using namespace Components;

InspectorPanel::InspectorPanel() : is_open{ true }
{
  auto& tex_manager = TexturesManager::instance();
  reset_icon_ = tex_manager.GetOrCreateIcon("reset-arrow-16.png");
}

void InspectorPanel::render(std::string_view window_name, Entity entity)
{
  if (!is_open || !scene_)
    return;

  ImGui::Begin(window_name.data(), &is_open);
  if (entity.Valid())
    list_all_components(entity);

  ImGui::End();
}

void InspectorPanel::list_all_components(Entity entity)
{
  auto& archetype_component = _scene->GetEntityComponent<ArchetypeIdentifier>(entity);
  auto archetype_id = archetype_component.archetypeId;
  auto& archetype = _scene->GetArchetypeRegistry().GetArchetype(archetype_id);
  auto archetype_name = archetype.GetName();

  ImGui::Text("Archetype: %s (ID %d)", archetype_name, archetype_id);
  ImGui::Spacing();

  auto& tag_component = _scene->GetEntityComponent<Tag>(entity);
  render_tag_component(tag_component);
  ImGui::Spacing();

  if (auto camera_component = scene_->TryGetEntityComponent<Camera>(entity))
  {
    if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick))
    {
      render_camera_component(*camera_component);
      ImGui::Spacing();
    }
  }
  if (auto transform_component = scene_->TryGetEntityComponent<Transform>(entity))
  {
    if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick))
    {
      render_transform_component(*transform_component);
      ImGui::Spacing();
    }
  }
  if (auto light_component = scene_->TryGetEntityComponent<Light>(entity))
  {
    if (ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick))
    {
      auto type = light_component->type;
      switch (type)
      {
        case LightType::Directional:
        {
          auto& dir_light_component = scene_->GetEntityComponent<DirectionalLight>(entity);
          render_directional_light(dir_light_component);
          break;
        }
        case LightType::Point:
        {
          auto& point_light_component = scene_->GetEntityComponent<PointLight>(entity);
          render_point_light(point_light_component);
          break;
        }
        case LightType::Spot:
        {
          auto& spot_light_component = scene_->GetEntityComponent<SpotLight>(entity);
          render_spot_light(spot_light_component);
          break;
        }
        default:
          break;
      }
      ImGui::Spacing();
    }
  }
  if (auto static_mesh_component = scene_->TryGetEntityComponent<StaticMesh>(entity))
  {
    if (ImGui::CollapsingHeader("Static mesh", ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick))
    {
      render_static_mesh_component(*static_mesh_component);
      ImGui::Spacing();
    }
  }
  if (auto material_component = scene_->TryGetEntityComponent<Material>(entity))
  {
    if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick))
    {
      render_material_component(*material_component);
      ImGui::Spacing();
    }
  }
}

void InspectorPanel::render_tag_component(Tag& tag_component) const
{
  ImGui::PushID("Tag component");
  auto tag_buffer = std::array<char, Tag::max_len>{};
  auto current_tag = std::string_view(tag_component.value.data());
  auto length_to_copy = std::min(current_tag.size(), sizeof(tag_buffer) - 1);
  std::copy_n(current_tag.begin(), length_to_copy, tag_buffer.data());
  tag_buffer[current_tag.size()] = '\0';

  auto width = ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x;
  ImGui::PushItemWidth(width);
  if (ImGui::InputText("##Tag", tag_buffer.data(), sizeof(tag_buffer), ImGuiInputTextFlags_EnterReturnsTrue))
  {
    auto new_tag_value = std::string_view(tag_buffer.data());
    if (new_tag_value.empty())
    {
      std::copy_n(current_tag.begin(), length_to_copy, tag_buffer.data());
      tag_buffer[current_tag.size()] = '\0';
    }
    else
    {
      tag_component.Update(new_tag_value);
    }
  }
  ImGui::PopItemWidth();

  if (ImGui::IsItemHovered())
    ImGui::SetTooltip("Press ENTER to confirm changes");
  ImGui::PopID();
}

void InspectorPanel::render_transform_component(Transform& transform) const
{
  constexpr auto format = std::array<std::string_view, 3>{ "x: %.2f", "y: %.2f", "z: %.2f" };
  drag_3f("Position", "transform position", transform.position, 0.1f, -FLT_MAX, FLT_MAX, format);
  drag_3f("Rotation", "transform rotation", transform.eulerAngles, 0.1f, -180.0f, 180.0f, format);
  drag_3f("Scale", "transform scale", transform.scale, 0.1f, 10.f, -10.f, format);
}

void InspectorPanel::render_camera_component(Camera& camera) const
{
  constexpr auto format1 = std::array<std::string_view, 3>{ "x: %.2f", "y: %.2f", "z: %.2f" };
  constexpr auto format2 = std::array<std::string_view, 3>{ "pitch: %.2f", "yaw: %.2f", "roll: %.2f" };
  drag_3f("Position", "camera position", camera.position, 0.1f, -FLT_MAX, FLT_MAX, format1);
  drag_3f("Rotation", "camera rotation", camera.eulerAngles, 0.1f, -180.0f, 180.0f, format2);
  slider_1f("Horizontal Field Of View", "camera fovh", camera.fovH, 30.f, 90.f, "%.2f (° degrees)");
  slider_1f("Near clip distance (z-near)", "camera near clip", camera.nearClip, 0.1f, 10.0f, "%.2f");
  slider_1f("Far clip distance (z-far)", "camera far clip", camera.farClip, camera.nearClip, 10.0f, "%.2f");
}

void InspectorPanel::_render_static_mesh_component(Components::StaticMesh& mesh) const
{
  ImGui::Text("Vertex Array:");
  if (mesh.vertexArray.Valid())
    ImGui::BulletText("Valid: Yes");
  else
    ImGui::BulletText("Valid: No");

  ImGui::Spacing();

  ImGui::Text("Vertex Buffer:");
  if (mesh.vertexBuffer.Valid())
  {
    ImGui::BulletText("Valid: Yes");
    ImGui::BulletText("Nr vertices: %u", mesh.numVertices);
    ImGui::BulletText("Size: %d bytes", mesh.vertexBuffer.GetBufferSize());
  }
  else
    ImGui::BulletText("Valid: No");

  ImGui::Spacing();

  ImGui::Text("Index Buffer:");
  if (mesh.indexBuffer.Valid())
  {
    ImGui::BulletText("Valid: Yes");
    ImGui::BulletText("Num Indices: %u", mesh.numIndices);
    ImGui::BulletText("Size: %u bytes", mesh.numIndices * sizeof(u32));
  }
  else
    ImGui::BulletText("Valid: No");
}

void InspectorPanel::render_material_component(Components::Material& material) const
{
  auto& instance = TexturesManager::GetInstance();
  ImGui::Text("Albedo");
  if (material.albedo.Valid())
  {
    auto path = instance.GetTexturePath(material.albedo.id);
    if (path)
      ImGui::BulletText("Path: %s", path.value()->string().c_str());
    else
      ImGui::BulletText("Path: <not found>");
    
    ImGui::Image(material.albedo.id, ImVec2(64, 64));
  }
  else
    ImGui::BulletText("None");

  ImGui::Text("Normal map");
  if (material.normalMap.Valid())
  {
    auto path = instance.GetTexturePath(material.normalMap.id);
    if (path)
      ImGui::BulletText("Path: %s", path.value()->string().c_str());
    else
      ImGui::BulletText("Path: <not found>");
    
    ImGui::Image(material.normalMap.id, ImVec2(64, 64));
  }
  else
    ImGui::BulletText("None");
}

void InspectorPanel::render_directional_light(Components::DirectionalLight& light) const
{
  ImGui::Text("Light type: DIRECTIONAL");
  ImGui::Spacing();

  constexpr auto format = std::array<std::string_view, 3>{ "x: %.2f", "y: %.2f", "z: %.2f" };
  color_edit_3("Color", "light color", light.color);
  
  auto tmp = Vec3F(light.direction);
  drag_3f("Direction", "light direction", tmp, 0.1f, -10.f, 10.f, format);
  light.direction = Vec4F(tmp, 0.0f);
  
  slider_1f("Intensity", "light intensity", light.intensity, 0.0f, 1.0f, "%.2f");
}

void InspectorPanel::render_point_light(Components::PointLight & light) const
{
  ImGui::Text("Light type: POINT");
  ImGui::Spacing();

  constexpr auto format = std::array<std::string_view, 3>{ "x: %.2f", "y: %.2f", "z: %.2f" };
  color_edit_3("Color", "light color", light.color);
  
  auto tmp = Vec3F(light.position);
  drag_3f("Position", "light position", tmp, 0.1f, -FLT_MAX, FLT_MAX, format);
  light.position = Vec4F(tmp, 0.0f);
  
  slider_1f("Intensity", "light intensity", light.intensity, 0.0f, 1.0f, "%.2f");
  
  attenuation_preset_combo("point attenuation", light.kl, light.kq);

  slider_1f("", "light kl", light.kl, 0.7f, 0.0014f, "Linear: %.6f");
  slider_1f("", "light kq", light.kq, 1.8f, 7e-6, "Quadratic: %.6f");
}

void InspectorPanel::render_spot_light(Components::SpotLight& light) const
{
  ImGui::Text("Light type: SPOT");
  ImGui::Spacing();
  
  constexpr auto format = std::array<std::string_view, 3>{ "x: %.2f", "y: %.2f", "z: %.2f" };
  color_edit_3("Color", "spot color", light.color);
  
  auto tmp = vec3f(light.position);
  drag_3f("Position", "spot position", tmp, 0.1f, -FLT_MAX, FLT_MAX, format);
  light.position = vec4f(tmp, 0.0f);

  tmp = vec3f(light.direction);
  drag_3f("Direction", "spot direction", tmp, 0.1f, -10.0f, 10.0f, format);
  light.direction = vec4f(tmp, 0.0f);

  slider_1f("Intensity", "spot intensity", light.intensity, 0.0f, 1.0f, "%.2f");
  attenuation_preset_combo("spot attenuation", light.kl, light.kq);
  
  slider_1f("Inner Angle (Degrees)", "spot thetaP", light.thetaP, 0.0f, 90.0f, "%.2f");
  slider_1f("Outer Angle (Degrees)", "spot thetaU", light.thetaU, light.thetaP, 90.0f, "%.2f");
}

void InspectorPanel::__attenuation_preset_combo(std::string_view id, f32& kl, f32& kq) const
{
  ImGui::PushID(id.data());
  ImGui::Text("Attenuation preset");

  auto current_selection = -1;
  auto idx = 0;
  for (const auto& arr : ATTENUATION_RANGES)
  {
    if (std::abs(arr[1] - kl) < 1e-4f && std::abs(arr[2] - kq) < 1e-6f)
    {
      current_selection = idx;
      break;
    }
    ++idx;
  }
  
  auto preview = std::array<char, 64>{};
  if (current_selection >= 0)
    std::format_to_n(preview.data(), sizeof(preview), "Range {}", i32((*std::next(ATTENUATION_RANGES.begin(), current_selection))[0]));
  else
    std::format_to_n(preview.data(), sizeof(preview), "Custom");

  auto width = (ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x);
  ImGui::PushItemWidth(width);
  if (ImGui::BeginCombo("##AttenuationPreset", preview.data()))
  {
    idx = 0;
    for (const auto& arr : ATTENUATION_RANGES)
    {
      auto label = std::array<char, 64>{};
      std::format_to_n(label.data(), sizeof(label), "Range {} ({},{})", i32(arr[0]), arr[1], arr[2]);      
      auto is_selected = (current_selection == idx);
      if (ImGui::Selectable(label.data(), is_selected))
      {
        kl = arr[1];
        kq = arr[2];
      }
      if (is_selected)
        ImGui::SetItemDefaultFocus();
      ++idx;
    }
    ImGui::EndCombo();
  }
  ImGui::PopItemWidth();
  ImGui::PopID();
}

void InspectorPanel::drag_3f(std::string_view label, 
                             std::string_view id,
                             f32 values[3], 
                             f32 speed, 
                             f32 min_val, 
                             f32 max_val, 
                             std::array<std::string_view, 3> format) const
{
  ImGui::PushID(id.data());
  if (!label.empty())
    ImGui::Text(label.data());
  auto width = (ImGui::GetContentRegionAvail().x - 3.0f * ImGui::GetStyle().ItemSpacing.x) / 3.0f;
  ImGui::PushItemWidth(width);
  ImGui::DragFloat("##X", &values[0], speed, min_val, max_val, format[0].data());
  ImGui::SameLine();
  ImGui::DragFloat("##Y", &values[1], speed, min_val, max_val, format[1].data());
  ImGui::SameLine();
  ImGui::DragFloat("##Z", &values[2], speed, min_val, max_val, format[2].data());
  ImGui::PopItemWidth();
  ImGui::PopID();
}

void InspectorPanel::drag_1f(std::string_view label, 
                             std::string_view id,
                             f32& value, 
                             f32 speed, 
                             f32 min_val, 
                             f32 max_val,
                             std::string_view format,
                             i32 flags) const
{
  ImGui::PushID(id.data());
  if (!label.empty())
    ImGui::Text(label.data());
  auto width = (ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x);
  ImGui::PushItemWidth(width);
  ImGui::DragFloat("##value", &value, speed, min_val, max_val, format.data(), flags);
  ImGui::PopItemWidth();
  ImGui::PopID();
}

void InspectorPanel::slider_1f(std::string_view label,
                               std::string_view id,
                                f32& value,
                                f32 min_val,
                                f32 max_val,
                                std::string_view format,
                                i32 flags) const
{
  ImGui::PushID(id.data());
  if(!label.empty())
    ImGui::Text(label.data());
  auto width = (ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x);
  ImGui::PushItemWidth(width);
  ImGui::SliderFloat("##value", &value, min_val, max_val, format.data(), flags);
  ImGui::PopItemWidth();
  ImGui::PopID();
}

void InspectorPanel::color_edit_3(std::string_view label, 
										              std::string_view id, 
										              f32 color[3]) const
{
  ImGui::PushID(id.data());
  if (!label.empty())
    ImGui::Text(label.data());
  ImGui::ColorEdit3("##color", color);
  ImGui::PopID();
}

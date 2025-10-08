#include "InspectorPanel.hpp"
#include "Engine/ECS/Scene.hpp"
#include "Engine/Managers/TexturesManager.hpp"
#include <imgui.h>

using namespace Components;

InspectorPanel::InspectorPanel() : 
  isOpen{ true }
{
  auto& texManager = TexturesManager::GetInstance();
  _resetIcon = texManager.GetOrCreateIcon("reset-arrow-16.png");
}

void InspectorPanel::Render(StringView windowName, Entity entity)
{
  if (!isOpen || !_scene)
    return;

  ImGui::Begin(windowName.data(), &isOpen);
  if (entity.Valid())
    __ListAllComponents(entity);

  ImGui::End();
}

void InspectorPanel::__ListAllComponents(Entity entity)
{
  auto& archetypeComponent = _scene->GetEntityComponent<ArchetypeIdentifier>(entity);
  auto archetypeId = archetypeComponent.archetypeId;
  auto& archetype = _scene->GetArchetypeRegistry().GetArchetype(archetypeId);
  auto archetypeName = archetype.GetName();

  ImGui::Text("Archetype: %s (ID %d)", archetypeName, archetypeId);
  ImGui::Spacing();

  auto& tagComponent = _scene->GetEntityComponent<Tag>(entity);
  __RenderTagComponent(tagComponent);
  ImGui::Spacing();

  if (auto cameraComponent = _scene->TryGetEntityComponent<Camera>(entity))
  {
    if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick))
    {
      __RenderCameraComponent(*cameraComponent);
      ImGui::Spacing();
    }
  }
  if (auto transformComponent = _scene->TryGetEntityComponent<Transform>(entity))
  {
    if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick))
    {
      __RenderTransformComponent(*transformComponent);
      ImGui::Spacing();
    }
  }
  if (auto lightComponent = _scene->TryGetEntityComponent<Light>(entity))
  {
    if (ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick))
    {
      auto type = lightComponent->type;
      switch (type)
      {
        case LightType::Directional:
        {
          auto& dirLightComponent = _scene->GetEntityComponent<DirectionalLight>(entity);
          __RenderDirectionalLight(dirLightComponent);
          break;
        }
        case LightType::Point:
        {
          auto& pointLightComponent = _scene->GetEntityComponent<PointLight>(entity);
          __RenderPointLight(pointLightComponent);
          break;
        }
        case LightType::Spot:
        {
          auto& spotLightComponent = _scene->GetEntityComponent<SpotLight>(entity);
          __RenderSpotLight(spotLightComponent);
          break;
        }
        default:
          break;
      }
      ImGui::Spacing();
    }
  }
  if (auto staticMeshComponent = _scene->TryGetEntityComponent<StaticMesh>(entity))
  {
    if (ImGui::CollapsingHeader("Static mesh", ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick))
    {
      __RenderStaticMeshComponent(*staticMeshComponent);
      ImGui::Spacing();
    }
  }
  if (auto materialComponent = _scene->TryGetEntityComponent<Material>(entity))
  {
    if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick))
    {
      __RenderMaterialComponent(*materialComponent);
      ImGui::Spacing();
    }
  }
}
void InspectorPanel::__RenderTagComponent(Tag& tagComponent) const
{
  ImGui::PushID("Tag component");
  auto tagBuffer = Array<char, 32>{};
  auto currentTag = StringView(tagComponent.value.data());
  auto lengthToCopy = std::min(currentTag.size(), sizeof(tagBuffer) - 1);
  std::copy_n(currentTag.begin(), lengthToCopy, tagBuffer.data());
  tagBuffer[currentTag.size()] = '\0';

  auto width = ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x;
  ImGui::PushItemWidth(width);
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
      tagComponent.Update(newTagValue);
    }
  }
  ImGui::PopItemWidth();

  if (ImGui::IsItemHovered())
    ImGui::SetTooltip("Press ENTER to confirm changes");
  ImGui::PopID();
}
void InspectorPanel::__RenderTransformComponent(Transform& transform) const
{
  constexpr auto format = Array<StringView, 3>{ "x: %.2f", "y: %.2f", "z: %.2f" };
  __Drag3F("Position", "transform position", transform.position, 0.1f, -FLT_MAX, FLT_MAX, format);
  __Drag3F("Rotation", "transform rotation", transform.eulerAngles, 0.1f, -180.0f, 180.0f, format);
  __Drag3F("Scale", "transform scale", transform.scale, 0.1f, 10.f, -10.f, format);
}
void InspectorPanel::__RenderCameraComponent(Camera& camera) const
{
  constexpr auto format1 = Array<StringView, 3>{ "x: %.2f", "y: %.2f", "z: %.2f" };
  constexpr auto format2 = Array<StringView, 3>{ "pitch: %.2f", "yaw: %.2f", "roll: %.2f" };
  __Drag3F("Position", "camera position", camera.position, 0.1f, -FLT_MAX, FLT_MAX, format1);
  __Drag3F("Rotation", "camera rotation", camera.eulerAngles, 0.1f, -180.0f, 180.0f, format2);
  __Slider1F("Horizontal Field Of View", "camera fovh", camera.fovH, 30.f, 90.f, "%.2f (° degrees)");
  __Slider1F("Near clip distance (z-near)", "camera near clip", camera.nearClip, 0.1f, 10.0f, "%.2f");
  __Slider1F("Far clip distance (z-far)", "camera far clip", camera.farClip, camera.nearClip, 10.0f, "%.2f");
}
void InspectorPanel::__RenderStaticMeshComponent(Components::StaticMesh& mesh) const
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
void InspectorPanel::__RenderMaterialComponent(Components::Material& material) const
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

void InspectorPanel::__RenderDirectionalLight(Components::DirectionalLight& light) const
{
  ImGui::Text("Light type: DIRECTIONAL");
  ImGui::Spacing();

  constexpr auto format = Array<StringView, 3>{ "x: %.2f", "y: %.2f", "z: %.2f" };
  __ColorEdit3("Color", "light color", light.color);
  
  auto tmp = Vec3F(light.direction);
  __Drag3F("Direction", "light direction", tmp, 0.1f, -10.f, 10.f, format);
  light.direction = Vec4F(tmp, 0.0f);
  
  __Slider1F("Intensity", "light intensity", light.intensity, 0.0f, 1.0f, "%.2f");
}
void InspectorPanel::__RenderPointLight(Components::PointLight & light) const
{
  ImGui::Text("Light type: POINT");
  ImGui::Spacing();

  constexpr auto format = Array<StringView, 3>{ "x: %.2f", "y: %.2f", "z: %.2f" };
  __ColorEdit3("Color", "light color", light.color);
  
  auto tmp = Vec3F(light.position);
  __Drag3F("Position", "light position", tmp, 0.1f, -FLT_MAX, FLT_MAX, format);
  light.position = Vec4F(tmp, 0.0f);
  
  __Slider1F("Intensity", "light intensity", light.intensity, 0.0f, 1.0f, "%.2f");
  
  __AttenuationPresetCombo("point attenuation", light.kl, light.kq);

  __Slider1F("", "light kl", light.kl, 0.7f, 0.0014f, "Linear: %.6f");
  __Slider1F("", "light kq", light.kq, 1.8f, 7e-6, "Quadratic: %.6f");
}
void InspectorPanel::__RenderSpotLight(Components::SpotLight& light) const
{
  ImGui::Text("Light type: SPOT");
  ImGui::Spacing();
  
  constexpr auto format = Array<StringView, 3>{ "x: %.2f", "y: %.2f", "z: %.2f" };
  __ColorEdit3("Color", "spot color", light.color);
  
  auto tmp = Vec3F(light.position);
  __Drag3F("Position", "spot position", tmp, 0.1f, -FLT_MAX, FLT_MAX, format);
  light.position = Vec4F(tmp, 0.0f);

  tmp = Vec3F(light.direction);
  __Drag3F("Direction", "spot direction", tmp, 0.1f, -10.0f, 10.0f, format);
  light.direction = Vec4F(tmp, 0.0f);

  __Slider1F("Intensity", "spot intensity", light.intensity, 0.0f, 1.0f, "%.2f");
  __AttenuationPresetCombo("spot attenuation", light.kl, light.kq);
  
  __Slider1F("Inner Angle (Degrees)", "spot thetaP", light.thetaP, 0.0f, 90.0f, "%.2f");
  __Slider1F("Outer Angle (Degrees)", "spot thetaU", light.thetaU, light.thetaP, 90.0f, "%.2f");
}
void InspectorPanel::__AttenuationPresetCombo(StringView id, f32& kl, f32& kq) const
{
  ImGui::PushID(id.data());
  ImGui::Text("Attenuation preset");

  auto currentSelection = -1;
  auto idx = 0;
  for (const auto& arr : ATTENUATION_RANGES)
  {
    if (std::abs(arr[1] - kl) < 1e-4f && std::abs(arr[2] - kq) < 1e-6f)
    {
      currentSelection = idx;
      break;
    }
    ++idx;
  }
  
  auto preview = Array<char, 64>{};
  if (currentSelection >= 0)
    std::format_to_n(preview.data(), sizeof(preview), "Range {}", i32((*std::next(ATTENUATION_RANGES.begin(), currentSelection))[0]));
  else
    std::format_to_n(preview.data(), sizeof(preview), "Custom");

  auto width = (ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x);
  ImGui::PushItemWidth(width);
  if (ImGui::BeginCombo("##AttenuationPreset", preview.data()))
  {
    idx = 0;
    for (const auto& arr : ATTENUATION_RANGES)
    {
      auto label = Array<char, 64>{};
      std::format_to_n(label.data(), sizeof(label), "Range {} ({},{})", i32(arr[0]), arr[1], arr[2]);      
      auto isSelected = (currentSelection == idx);
      if (ImGui::Selectable(label.data(), isSelected))
      {
        kl = arr[1];
        kq = arr[2];
      }
      if (isSelected)
        ImGui::SetItemDefaultFocus();
      ++idx;
    }
    ImGui::EndCombo();
  }
  ImGui::PopItemWidth();
  ImGui::PopID();
}

void InspectorPanel::__Drag3F(StringView label, 
                              StringView id,
                              Vec3F& values, 
                              f32 speed, 
                              f32 minVal, 
                              f32 maxVal, 
                              Array<StringView, 3> format) const
{
  ImGui::PushID(id.data());
  if (!label.empty())
    ImGui::Text(label.data());
  auto width = (ImGui::GetContentRegionAvail().x - 3.0f * ImGui::GetStyle().ItemSpacing.x) / 3.0f;
  ImGui::PushItemWidth(width);
  ImGui::DragFloat("##X", &values.x, speed, minVal, maxVal, format[0].data());
  ImGui::SameLine();
  ImGui::DragFloat("##Y", &values.y, speed, minVal, maxVal, format[1].data());
  ImGui::SameLine();
  ImGui::DragFloat("##Z", &values.z, speed, minVal, maxVal, format[2].data());
  ImGui::PopItemWidth();
  ImGui::PopID();
}

void InspectorPanel::__Drag1F(StringView label, 
                              StringView id,
                              f32& value, 
                              f32 speed, 
                              f32 minVal, 
                              f32 maxVal,
                              StringView format,
                              i32 flags) const
{
  ImGui::PushID(id.data());
  if (!label.empty())
    ImGui::Text(label.data());
  auto width = (ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x);
  ImGui::PushItemWidth(width);
  ImGui::DragFloat("##value", &value, speed, minVal, maxVal, format.data(), flags);
  ImGui::PopItemWidth();
  ImGui::PopID();
}

void InspectorPanel::__Slider1F(StringView label,
                                StringView id,
                                f32& value,
                                f32 minVal,
                                f32 maxVal,
                                StringView format,
                                i32 flags) const
{
  ImGui::PushID(id.data());
  if(!label.empty())
    ImGui::Text(label.data());
  auto width = (ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x);
  ImGui::PushItemWidth(width);
  ImGui::SliderFloat("##value", &value, minVal, maxVal, format.data(), flags);
  ImGui::PopItemWidth();
  ImGui::PopID();
}

void InspectorPanel::__ColorEdit3(StringView label, StringView id, Vec4F& color) const
{
  ImGui::PushID(id.data());
  if (!label.empty())
    ImGui::Text(label.data());
  ImGui::ColorEdit3("##color", &color.r);
  ImGui::PopID();
}


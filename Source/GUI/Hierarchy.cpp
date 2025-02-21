#include "ImGuiLayer.hpp"

#include "Engine/Scene.hpp"
#include "Engine/ECS/ECS.hpp"
#include "Engine/Utils.hpp"
#include "Engine/Subsystems/ShadersManager.hpp"
#include "Engine/Subsystems/TexturesManager.hpp"
#include "Engine/Filesystem/Filesystem.hpp"

#include <imgui/imgui.h>

// ----------------------------------------------------
//          PRIVATE          
// ----------------------------------------------------

static bool ButtonCentered(const char* label, ImVec2 size)
{
  f32 avail = ImGui::GetContentRegionAvail().x;
  f32 off = (avail - size.x) * 0.5f;
  if (off > 0.0f)
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);

  return ImGui::Button(label, size);
}

static void Hierarchy_ListObjects(Scene& scene, GameObject& objSelected)
{
  auto& texManager = TexturesManager::Get();
  static const auto* icon = texManager.GetOrCreateIcon("game-object-16.png");

  char selectableName[64]{};
  for (auto [entity, tag] : scene.Reg().view<Tag>().each())
  {
    GameObject o{ entity, &scene.Reg() };
    
    std::fill_n(selectableName, sizeof(selectableName), 0);
    std::format_to_n(selectableName, sizeof(selectableName), "{}##{}", tag.value, static_cast<u32>(entity));

    ImGui::BeginGroup();
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + (ImGui::GetTextLineHeight() - 16.f) / 2);
    
    ImGui::Image(reinterpret_cast<void*>(icon->id), ImVec2(16.f, 16.f));
    ImGui::SameLine();
    
    auto& colors = ImGui::GetStyle().Colors;
    ImVec4 colorSelected = colors[ImGuiCol_FrameBgActive];
    ImVec4 colorHovered = colors[ImGuiCol_FrameBgHovered];
    ImGui::PushStyleColor(ImGuiCol_Header, colorSelected);
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, colorHovered);
    ImGui::PushStyleColor(ImGuiCol_HeaderActive, colorSelected);

    bool selected = ImGui::Selectable(selectableName, objSelected.Compare(o));
    if (selected && !objSelected.Compare(o))
      objSelected = o;
    
    if (
      ImGui::IsItemHovered() &&
      ImGui::IsMouseClicked(ImGuiMouseButton_Right) &&
      objSelected.IsValid()
    )
      ImGui::OpenPopup("ObjectMenu");
    
    ImGui::PopStyleColor(3);
    ImGui::EndGroup();

    ImGui::Spacing();
  }
}
static void Hierarchy_ObjectMenuPopup(Scene& scene, GameObject& objSelected)
{
  if (ImGui::BeginPopup("ObjectMenu"))
  {
    if (ImGui::MenuItem("Delete object"))
    {
      // Unset light component
      if (objSelected.HasComponent<Light>())
      {
        ShadersManager& shadersManager = ShadersManager::Get();
        auto& shaderScene = shadersManager.GetProgram("Scene");
        auto& shaderSceneShadows = shadersManager.GetProgram("SceneShadows");

        if (objSelected.HasComponent<DirectionalLight>())
        {
          shaderScene.SetUniform1f("u_directionalLight.intensity", 0.f);
          shaderSceneShadows.SetUniform1f("u_directionalLight.intensity", 0.f);
        }
        else if (objSelected.HasComponent<PointLight>())
        {
          shaderScene.SetUniform1f("u_pointLight.intensity", 0.f);
          shaderSceneShadows.SetUniform1f("u_pointLight.intensity", 0.f);
        }
        else if (objSelected.HasComponent<SpotLight>())
        {
          shaderScene.SetUniform1f("u_spotLight.intensity", 0.f);
          shaderSceneShadows.SetUniform1f("u_spotLight.intensity", 0.f);
        }
      }

      scene.DestroyObject(objSelected.id);
    }
    ImGui::EndPopup();
  }
}

// ----------------------------------------------------
//          PUBLIC           
// ----------------------------------------------------

void GUI_RenderHierarchy(bool& open, Scene& scene, GameObject& objSelected)
{
  static bool createNewObject = false;

  ImGui::Begin("Hierarchy", &open);

  /* "+New object" button */
  f32 btnWidth = ImGui::GetContentRegionAvail().x - 32.f;
  if(ButtonCentered("+New object", ImVec2(btnWidth, 26.f)))
    scene.CreateObject();
  
  ImGui::Spacing();
  ImGui::Separator();
  ImGui::Spacing();

  /* List all entities */
  Hierarchy_ListObjects(scene, objSelected);

  /* Display menu on right click object */
  Hierarchy_ObjectMenuPopup(scene, objSelected);
  
  ImGui::End();
}

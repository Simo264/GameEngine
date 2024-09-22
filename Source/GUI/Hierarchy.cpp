#include "Core/Core.hpp"
#include "Engine/Scene.hpp"
#include "Engine/ECS/ECS.hpp"
#include "Engine/Subsystems/ShaderManager.hpp"
#include "Engine/Subsystems/TextureManager.hpp"
#include "Engine/Filesystem/Dialog.hpp"

#include "ImGuiLayer.hpp"

#include <imgui/imgui.h>

/* -------------------------- */
/*          PRIVATE           */
/* -------------------------- */

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
  char selectableName[64]{};
  for (auto [entity, tag] : scene.Reg().view<Tag>().each())
  {
    GameObject o{ entity, &scene.Reg() };
    
    std::fill_n(selectableName, sizeof(selectableName), 0);
    std::format_to_n(selectableName, sizeof(selectableName), "{}##{}", tag.value, static_cast<u32>(entity));

    ImGui::BeginGroup();
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + (ImGui::GetTextLineHeight() - 16.f) / 2);
    
    const auto& icon = TextureManager::Get().GetIconByPath(GetIconsPath() / "game-object-16.png");
    ImGui::Image(reinterpret_cast<void*>(icon.id), ImVec2(16.f, 16.f));
    ImGui::SameLine();
    
    auto& colors = ImGui::GetStyle().Colors;
    ImVec4 colorSelected = colors[ImGuiCol_FrameBgActive];
    ImVec4 colorHovered = colors[ImGuiCol_FrameBgHovered];
    ImGui::PushStyleColor(ImGuiCol_Header, colorSelected);
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, colorHovered);
    ImGui::PushStyleColor(ImGuiCol_HeaderActive, colorSelected);

    bool selected = ImGui::Selectable(selectableName, objSelected.IsEqual(o));
    if (selected && !objSelected.IsEqual(o))
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
      scene.DestroyObject(objSelected);
      objSelected.Invalidate();

      ShaderManager::Get().GetProgramByName("Scene").Link();
      ShaderManager::Get().GetProgramByName("SceneShadows").Link();
      ShaderManager::Get().ResetProgramsUniforms();
    }
    ImGui::EndPopup();
  }
}

/* -------------------------- */
/*          PUBLIC            */
/* -------------------------- */

void GUI_RenderHierarchy(bool& open, Scene& scene, GameObject& objSelected)
{
  static bool createNewObject = false;

  ImGui::Begin("Outliner", &open);

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

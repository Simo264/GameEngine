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


static bool Hierarchy_NewObjectButton()
{
  ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
  f32 width = ImGui::GetContentRegionAvail().x;
  bool clicked = ImGui::Button("+New object", ImVec2(width, 30.f));
  ImGui::PopStyleVar();
  return clicked;
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
    
    const auto& icon = TextureManager::Get().GetIconByPath(GetIconsPath() / "game-object-16px.png");
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
  if(Hierarchy_NewObjectButton())
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

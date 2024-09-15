#include "Core/Core.hpp"
#include "Engine/Scene.hpp"
#include "Engine/ECS/ECS.hpp"

#include "ImGuiLayer.hpp"

#include <imgui/imgui.h>

static char nodeName[64]{};

void GUI_RenderOutliner(bool& open, Scene& scene, GameObject& objSelected)
{
  ImGui::Begin("Outliner", &open);
  ImGui::SetNextItemOpen(true, ImGuiCond_Once);
  for (auto [entity, label] : scene.Reg().view<Label>().each())
  {
    ImGuiTreeNodeFlags flags =
      ImGuiTreeNodeFlags_OpenOnArrow |
      ImGuiTreeNodeFlags_OpenOnDoubleClick |
      ImGuiTreeNodeFlags_SpanAvailWidth |
      ImGuiTreeNodeFlags_Leaf |
      ImGuiTreeNodeFlags_NoTreePushOnOpen;
    
    GameObject o{ entity, &scene.Reg() };
    if (objSelected.IsValid() && objSelected.IsEqual(o))
      flags |= ImGuiTreeNodeFlags_Selected;

    std::format_to_n(nodeName, sizeof(nodeName), "{}", label.value.c_str());
    ImGui::TreeNodeEx(reinterpret_cast<void*>(entity), flags, nodeName);

    if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
      objSelected = o;

    std::fill_n(nodeName, sizeof(nodeName), 0);
  }
  ImGui::End();
}

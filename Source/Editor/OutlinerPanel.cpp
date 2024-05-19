#include "OutlinerPanel.hpp"

#include "Core/Log/Logger.hpp"

#include "Engine/Scene.hpp"
#include "Engine/Graphics/Texture2D.hpp"
#include "Engine/Subsystems/TextureManager.hpp"

#include "Engine/GameObject.hpp"
#include "Engine/Components.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>



/* -----------------------------------------------------
 *          PUBLIC METHODS
 * -----------------------------------------------------
*/

OutlinerPanel::OutlinerPanel(const char* panelName, bool visible)
  : Panel(panelName, visible),
    _iconSize{ 16.0f },
    _selected{} /* null entity */
{}

#if 0 
void OutlinerPanel::RenderPanel(Scene& scene)
{
  ImGui::Begin(panelName.c_str(), &visible);

  AddSceneComponentButton("PopupAddElement");
  if (ImGui::BeginPopup("PopupAddElement"))
    AddSceneComponentPopup(scene);

  ImGui::SetNextItemOpen(true, ImGuiCond_Once);
  if (ImGui::TreeNode("Scene"))
  {
    /* Iterate over each object. Every object has LabelComponent and TypeComponent */
    auto view = scene.Reg().view<LabelComponent, TypeComponent>();
    for (auto [entity, labelComp, typeComp] : view.each())
    {
      ImGui::SetNextItemOpen(true, ImGuiCond_Once);

      const string& label = labelComp.label;
      GameObject object{ entity, &scene.Reg() };

      ImGuiTreeNodeFlags nodeFlag = ImGuiTreeNodeFlags_OpenOnArrow;
      if (_selected.IsValid() && _selected.Compare(object))
        nodeFlag |= ImGuiTreeNodeFlags_Selected;
      
      const bool nodeOpen = ImGui::TreeNodeEx(reinterpret_cast<void*>(entity), nodeFlag, label.c_str());
      if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
        _selected = object;

      if (nodeOpen)
      {
        const GameObjectType type = static_cast<GameObjectType>(typeComp.type);
        const char* typeName = GetTypeName(type);
        ImGui::TextDisabled("Type: %s", typeName);
        ImGui::TreePop();
      }
    }
    ImGui::TreePop();
  }


  ImGui::End(); /* End frame */
}


/* -----------------------------------------------------
 *          PRIVATE METHODS
 * -----------------------------------------------------
*/

const char* OutlinerPanel::GetTypeName(GameObjectType objectType)
{
  switch (objectType)
  {
  case GameObjectType::DIRECTIONAL_LIGHT:
    return "directional light";
    
  //case GameObjectType::POINT_LIGHT:
  //  return "point light";

  //case GameObjectType::SPOT_LIGHT:
  //  return "spot light";
    
  case GameObjectType::STATIC_MESH:
    return "static mesh";

  default:
    return "game object";
  }
}

void OutlinerPanel::AddSceneComponentButton(const char* labelPopup)
{
  if (ImGui::Button("Add item", { ImGui::GetContentRegionAvail().x, 24 }))
    ImGui::OpenPopup(labelPopup);

  ImGui::InvisibleButton("##margin-bottom", { 4,4 });
  ImGui::Separator();
  ImGui::InvisibleButton("##margin-bottom", { 4,4 });
}

void OutlinerPanel::AddSceneComponentPopup(Scene& scene)
{
  static const char* lights[] = { "Directional light", "Point light", "Spot light", };
  static const char* meshes[] = { "Cube", "Plane", "Cylinder", };
  
  ImGui::SeparatorText("Lighting");
  for (int i = 0; i < IM_ARRAYSIZE(lights); i++)
  {
    if (ImGui::Selectable(lights[i]))
    {
      switch (i)
      {
      case 0: /* Directional light */
      {
        if (scene.Reg().view<DirLightComponent>().size() == 0)
        {
          GameObject dLight = scene.CreateObject(
            "Directional light",
            static_cast<uint32_t>(GameObjectType::DIRECTIONAL_LIGHT));

          //dLight.AddComponent<DirLightComponent>(SHADER_UNIFORM_DIRLIGHT);
        }
        else
          CONSOLE_WARN("Can't create more dir light objects");

        break;
      }
      case 1: /* Pointlight */
      {
        //uint32_t size = scene.Reg().view<PointLightComponent>().size();
        //if (size < SHADER_MAX_NUM_POINTLIGHTS)
        //{
        //  GameObject pLight = scene.CreateObject(
        //    "Point light",
        //    static_cast<uint32_t>(GameObjectType::POINT_LIGHT));

        //  pLight.AddComponent<PointLightComponent>(SHADER_UNIFORM_POINTLIGHT(size));
        //}
        //else
        //  CONSOLE_WARN("Can't create more point light objects");
        
        CONSOLE_WARN("TODO");
        break;
      }
      case 2: /* Spotlight */
      {
        CONSOLE_WARN("TODO");
        break;
      }
      }
    }
  }

  ImGui::SeparatorText("Static mesh");
  for (int i = 0; i < IM_ARRAYSIZE(meshes); i++)
  {
    
    if (ImGui::Selectable(meshes[i]))
    {
      switch (i)
      {
      case 0: /* Cube mesh */
      {
        fspath model = ASSETS_PATH / (fspath("Shapes/Cube/Cube.obj").lexically_normal());
        GameObject cube = scene.CreateObject("Cube", static_cast<uint32_t>(GameObjectType::STATIC_MESH));
        cube.AddComponent<StaticMeshComponent>(model);
        cube.AddComponent<TransformComponent>();
        break;
      }
      case 1: /* Plane mesh */
      {
        fspath model = ASSETS_PATH / (fspath("Shapes/Plane/Plane.obj").lexically_normal());
        GameObject cube = scene.CreateObject("Plane", static_cast<uint32_t>(GameObjectType::STATIC_MESH));
        cube.AddComponent<StaticMeshComponent>(model);
        cube.AddComponent<TransformComponent>();
        break;
      }
      case 2: /* Cylinder mesh */
      {
        fspath model = ASSETS_PATH / (fspath("Shapes/Cylinder/Cylinder.obj").lexically_normal());
        GameObject cube = scene.CreateObject("Cylinder", static_cast<uint32_t>(GameObjectType::STATIC_MESH));
        cube.AddComponent<StaticMeshComponent>(model);
        cube.AddComponent<TransformComponent>();
        break;
      }
      }
    }
  }
  ImGui::EndPopup();
}

#endif
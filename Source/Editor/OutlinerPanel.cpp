#include "OutlinerPanel.hpp"

#include "Core/Log/Logger.hpp"

#include "Engine/Scene.hpp"
#include "Engine/Graphics/Texture2D.hpp"
#include "Engine/Subsystems/TextureManager.hpp"

#include "Engine/GameObjectType.hpp"
#include "Engine/ECS/LabelComponent.hpp"
#include "Engine/ECS/TypeComponent.hpp"
#include "Engine/ECS/StaticMeshComponent.hpp"
#include "Engine/ECS/TransformComponent.hpp"
#include "Engine/ECS/Lighting/DirLightComponent.hpp"
#include "Engine/ECS/Lighting/PointLightComponent.hpp"
#include "Engine/ECS/Lighting/SpotLightComponent.hpp"

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
{
  auto& instanceTM = TextureManager::Instance();
  _icons[static_cast<int>(ICON_TYPE::DIR_LIGHT)]    = instanceTM.GetTextureByPath(ICONS_PATH / "icon-dirlight.png");
  _icons[static_cast<int>(ICON_TYPE::POINT_LIGHT)]  = instanceTM.GetTextureByPath(ICONS_PATH / "icon-pointlight.png");
  _icons[static_cast<int>(ICON_TYPE::SPOT_LIGHT)]   = instanceTM.GetTextureByPath(ICONS_PATH / "icon-spotlight.png");
  _icons[static_cast<int>(ICON_TYPE::STATIC_MESH)]  = instanceTM.GetTextureByPath(ICONS_PATH / "icon-staticmesh.png");
  _icons[static_cast<int>(ICON_TYPE::VISIBLE)]      = instanceTM.GetTextureByPath(ICONS_PATH / "icon-visible.png");
  _icons[static_cast<int>(ICON_TYPE::HIDDEN)]       = instanceTM.GetTextureByPath(ICONS_PATH / "icon-hidden.png");
}

void OutlinerPanel::RenderPanel(Scene* scene)
{
  ImGui::Begin(panelName.c_str(), &visible);

  AddSceneComponentButton("PopupAddElement");
  if (ImGui::BeginPopup("PopupAddElement"))
    AddSceneComponentPopup(scene);

  if (ImGui::BeginTable("Table", 3, ImGuiTableFlags_BordersV))
  {
    ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 0.1f * ImGui::GetContentRegionAvail().x);   /* 10% width */
    ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 0.7f * ImGui::GetContentRegionAvail().x);   /* 70% width */
    ImGui::TableSetupColumn("##Visible", ImGuiTableColumnFlags_WidthFixed, 0.2f * ImGui::GetContentRegionAvail().x);   /* 20% width */
    ImGui::TableHeadersRow();

    /* Every object has LabelComponent and TypeComponent */
    auto view = scene->Reg().view<LabelComponent, TypeComponent>();
    for(auto [entity, labelComp, typeComp] : view.each())
    {
      const String& label = labelComp.label;
      const uint32_t itype = typeComp.type;
      const GameObjectType type = static_cast<GameObjectType>(itype);
      GameObject object{ entity, &scene->Reg() };

      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      Texture2D* icon = GetObjectIcon(itype);
      ImGui::Image((ImTextureID)icon->textureID, { _iconSize,_iconSize });
      SetIconTooltip(itype);

      ImGui::TableSetColumnIndex(1);
      ImGui::PushID((uint32_t)entity);
      if (ImGui::Selectable(label.c_str(), _selected.GetObjectID() == entity))
        _selected = object;
      ImGui::PopID();

      ImGui::TableSetColumnIndex(2);
      ImGui::ImageButton((ImTextureID)_icons[static_cast<int>(ICON_TYPE::VISIBLE)]->textureID, 
        { _iconSize,_iconSize });
    }
    ImGui::EndTable();
  }
  ImGui::End();
}


/* -----------------------------------------------------
 *          PRIVATE METHODS
 * -----------------------------------------------------
*/

Texture2D* OutlinerPanel::GetObjectIcon(uint32_t objectType)
{
  GameObjectType type = static_cast<GameObjectType>(objectType);
  switch (type)
  {
  case GameObjectType::DIRECTIONAL_LIGHT:
    return _icons[static_cast<int>(ICON_TYPE::DIR_LIGHT)];

  case GameObjectType::POINT_LIGHT:
    return _icons[static_cast<int>(ICON_TYPE::POINT_LIGHT)];

  case GameObjectType::SPOT_LIGHT:
    return _icons[static_cast<int>(ICON_TYPE::SPOT_LIGHT)];
  
  case GameObjectType::STATIC_MESH:
    return _icons[static_cast<int>(ICON_TYPE::STATIC_MESH)];

  default:
    CONSOLE_WARN("Object type %d does not have icon", objectType);
    return nullptr;
  }
}

void OutlinerPanel::SetIconTooltip(uint32_t objectType)
{
  GameObjectType type = static_cast<GameObjectType>(objectType);
  char text[32]{};
  if (ImGui::BeginItemTooltip())
  {
    switch (type)
    {
    case GameObjectType::DIRECTIONAL_LIGHT:
      sprintf_s(text, "Directional light");
      break;
    
    case GameObjectType::POINT_LIGHT:
      sprintf_s(text, "Point light");
      break;

    case GameObjectType::SPOT_LIGHT:
      sprintf_s(text, "Spot light");
      break;
    
    case GameObjectType::STATIC_MESH:
      sprintf_s(text, "Static mesh");
      break;
    }
    ImGui::TextUnformatted(text);
    ImGui::EndTooltip();
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

void OutlinerPanel::AddSceneComponentPopup(Scene* scene)
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
        if (scene->Reg().view<DirLightComponent>().size() == 0)
        {
          GameObject dLight = scene->CreateObject(
            "Directional light",
            static_cast<uint32_t>(GameObjectType::DIRECTIONAL_LIGHT));

          dLight.AddComponent<DirLightComponent>(SHADER_UNIFORM_DIRLIGHT);
        }
        else
          CONSOLE_WARN("Can't create more dir light objects");

        break;
      }
      case 1: /* Pointlight */
      {
        uint32_t size = scene->Reg().view<PointLightComponent>().size();
        if (size < MAX_NUM_POINTLIGHTS)
        {
          GameObject pLight = scene->CreateObject(
            "Point light",
            static_cast<uint32_t>(GameObjectType::POINT_LIGHT));

          pLight.AddComponent<PointLightComponent>(SHADER_UNIFORM_POINTLIGHT(size));
        }
        else
          CONSOLE_WARN("Can't create more point light objects");
        break;
      }
      case 2: /* TODO: Spotlight */
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
        Path model = ASSETS_PATH / (Path("Shapes/Cube/Cube.obj").lexically_normal());
        GameObject cube = scene->CreateObject("Cube", static_cast<uint32_t>(GameObjectType::STATIC_MESH));
        cube.AddComponent<StaticMeshComponent>(model);
        cube.AddComponent<TransformComponent>();
        break;
      }
      case 1: /* Plane mesh */
      {
        Path model = ASSETS_PATH / (Path("Shapes/Plane/Plane.obj").lexically_normal());
        GameObject cube = scene->CreateObject("Plane", static_cast<uint32_t>(GameObjectType::STATIC_MESH));
        cube.AddComponent<StaticMeshComponent>(model);
        cube.AddComponent<TransformComponent>();
        break;
      }
      case 2: /* Cylinder mesh */
      {
        Path model = ASSETS_PATH / (Path("Shapes/Cylinder/Cylinder.obj").lexically_normal());
        GameObject cube = scene->CreateObject("Cylinder", static_cast<uint32_t>(GameObjectType::STATIC_MESH));
        cube.AddComponent<StaticMeshComponent>(model);
        cube.AddComponent<TransformComponent>();
        break;
      }
      }
    }
  }
  ImGui::EndPopup();
}

#if 0
void OutlinerPanel::ToggleVisibility(SharedPointer<DirectionalLight>& dLight)
{
  auto& instanceTM = TextureManager::Instance();
  if (dLight->visible)
  {
    auto iconEye = instanceTM.GetTextureByPath(ICONS_PATH / "icon-eye.png");
    if (ImGui::ImageButtonEx(_buttonEyeID++, (ImTextureID)iconEye->textureID, { _iconSize,_iconSize }, { 0,0 }, { 1,1 }, { 0,0,0,0 }, { 1,1,1,1 }))
      dLight->visible = false;
  }
  else
  {
    auto iconHiddenEye = instanceTM.GetTextureByPath(ICONS_PATH / "icon-eye-hidden.png");
    if (ImGui::ImageButtonEx(_buttonEyeID++, (ImTextureID)iconHiddenEye->textureID, { _iconSize,_iconSize }, { 0,0 }, { 1,1 }, { 0,0,0,0 }, { 1,1,1,1 }))
      dLight->visible = true;
  }
  
  //auto shader = ShaderManager::Instance().GetShader("SceneShader");
  //shader->SetBool("UDirLightVisible", dLight->visible);
}

void OutlinerPanel::ToggleVisibility(SharedPointer<PointLight>& pLight)
{
  auto& instanceTM = TextureManager::Instance();
  if (pLight->visible)
  {
    auto iconEye = instanceTM.GetTextureByPath(ICONS_PATH / "icon-eye.png");
    if (ImGui::ImageButtonEx(_buttonEyeID++, (ImTextureID)iconEye->textureID, { _iconSize,_iconSize }, { 0,0 }, { 1,1 }, { 0,0,0,0 }, { 1,1,1,1 }))
      pLight->visible = false;
  }
  else
  {
    auto iconHiddenEye = instanceTM.GetTextureByPath(ICONS_PATH / "icon-eye-hidden.png");
    if (ImGui::ImageButtonEx(_buttonEyeID++, (ImTextureID)iconHiddenEye->textureID, { _iconSize,_iconSize }, { 0,0 }, { 1,1 }, { 0,0,0,0 }, { 1,1,1,1 }))
      pLight->visible = true;
  }

  //char uniform[32];
  //sprintf_s(uniform, "UPointLightVisible[%d]", sceneObj->GetID());
  //auto shader = ShaderManager::Instance().GetShader("SceneShader");
  //shader->SetInt(uniform, (int) sceneObj->visible);
}

void OutlinerPanel::ToggleVisibility(SharedPointer<StaticMesh>& sMesh)
{
  auto& instanceTM = TextureManager::Instance();
  if (sMesh->visible)
  {
    auto iconEye = instanceTM.GetTextureByPath(ICONS_PATH / "icon-eye.png");
    if (ImGui::ImageButtonEx(_buttonEyeID++, (ImTextureID)iconEye->textureID, { _iconSize,_iconSize }, { 0,0 }, { 1,1 }, { 0,0,0,0 }, { 1,1,1,1 }))
      sMesh->visible = false;
  }
  else
  {
    auto iconHiddenEye = instanceTM.GetTextureByPath(ICONS_PATH / "icon-eye-hidden.png");
    if (ImGui::ImageButtonEx(_buttonEyeID++, (ImTextureID)iconHiddenEye->textureID, { _iconSize,_iconSize }, { 0,0 }, { 1,1 }, { 0,0,0,0 }, { 1,1,1,1 }))
      sMesh->visible = true;
  }
}
#endif
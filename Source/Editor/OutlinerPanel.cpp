#include "OutlinerPanel.hpp"
#include "Engine/Scene.hpp"
#include "Engine/Graphics/Shader.hpp"
#include "Engine/Subsystems/TexturesManager.hpp"
#include "Engine/Subsystems/ShadersManager.hpp"
#include "Logger.hpp"

#include "Imgui/imgui.h"
#include "Imgui/imgui_internal.h"

/* -----------------------------------------------------
 *          PUBLIC METHODS
 * -----------------------------------------------------
*/

OutlinerPanel::OutlinerPanel(const char* panelName)
{
  this->panelName = panelName;
  isOpen = true;

  _iconSize = 16.0f;
  _buttonEyeID = 1;

  _dLightSelected = nullptr;
  _pLightSelected = nullptr;
  _sMeshSelected = nullptr;

  auto& instanceTM = TexturesManager::Instance();
  _icons[SUN] = instanceTM.GetTextureByPath(ROOT_PATH / "Icons/icon-sun.png");
  _icons[LAMP] = instanceTM.GetTextureByPath(ROOT_PATH / "Icons/icon-lamp.png");
  _icons[EYE] = instanceTM.GetTextureByPath(ROOT_PATH / "Icons/icon-eye.png");
  _icons[EYE_HIDDEN] = instanceTM.GetTextureByPath(ROOT_PATH / "Icons/icon-eye-hidden.png");
  _icons[MESH] = instanceTM.GetTextureByPath(ROOT_PATH / "Icons/icon-mesh.png");
  _icons[PLUS] = instanceTM.GetTextureByPath(ROOT_PATH / "Icons/icon-plus.png");
}

template<>
SharedPointer<DirectionalLight>& OutlinerPanel::GetItemSelected() { return _dLightSelected; }

template<>
SharedPointer<PointLight>& OutlinerPanel::GetItemSelected() { return _pLightSelected; }

template<>
SharedPointer<StaticMesh>& OutlinerPanel::GetItemSelected() { return _sMeshSelected; }

void OutlinerPanel::RenderPanel(Scene* scene)
{
  ImGui::Begin(panelName.c_str(), &isOpen);

  AddSceneComponentButton("PopupAddElement");
  if (ImGui::BeginPopup("PopupAddElement"))
    AddSceneComponentPopup(scene);

  ImGui::PushStyleColor(ImGuiCol_Button, { 0,0,0,0 });
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.25f,0.25f,0.25f, 0.75f });
  ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.25f,0.25f,0.25f, 0.75f });
  if (ImGui::BeginTable("Table", 3, ImGuiTableFlags_RowBg))
  {
    ImGui::TableSetupColumn("##C1", ImGuiTableColumnFlags_WidthFixed, 0.1f * ImGui::GetContentRegionAvail().x);   /* 10% width */
    ImGui::TableSetupColumn("##C2", ImGuiTableColumnFlags_WidthFixed, 0.7f * ImGui::GetContentRegionAvail().x);   /* 70% width */
    ImGui::TableSetupColumn("##C3", ImGuiTableColumnFlags_WidthFixed, 0.2f * ImGui::GetContentRegionAvail().x);   /* 20% width */

    _buttonEyeID = 1;

    /* Directional light row */
    if (scene->HasDirLight())
    {
      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Image((ImTextureID)_icons[SUN]->textureID, { _iconSize,_iconSize });
      ImGui::TableSetColumnIndex(1);

      ImGui::PushID(scene->sceneDLight->GetID());
      if (ImGui::Selectable(scene->sceneDLight->tagName.c_str(), (_dLightSelected != nullptr)))
      {
        _dLightSelected = scene->sceneDLight;
        _pLightSelected = nullptr;
        _sMeshSelected = nullptr;
      }
      ImGui::PopID();
      ImGui::TableSetColumnIndex(2);
      ToggleVisibility(scene->sceneDLight);
    }

    /* Point light rows */
    if (scene->HasPointLights())
    {
      for (auto& scenePLight : scene->scenePLights)
      {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Image((ImTextureID)_icons[LAMP]->textureID, { _iconSize,_iconSize });
        ImGui::TableSetColumnIndex(1);
        ImGui::PushID(scenePLight->GetID());
        if (ImGui::Selectable(scenePLight->tagName.c_str(), (_pLightSelected == scenePLight)))
        {
          _dLightSelected = nullptr;
          _pLightSelected = scenePLight;
          _sMeshSelected = nullptr;
        }
        ImGui::PopID();
        ImGui::TableSetColumnIndex(2);
        ToggleVisibility(scenePLight);
      }
    }

    /* Static mesh rows */
    if (scene->HasStaticMeshes())
    {
      for (auto& sceneMesh : scene->sceneSMeshes)
      {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Image((ImTextureID)_icons[MESH]->textureID, { _iconSize,_iconSize });
        ImGui::TableSetColumnIndex(1);
        ImGui::PushID(sceneMesh->GetID());
        if (ImGui::Selectable(sceneMesh->tagName.c_str(), (_sMeshSelected == sceneMesh)))
        {
          _dLightSelected = nullptr;
          _pLightSelected = nullptr;
          _sMeshSelected = sceneMesh;
        }
        ImGui::PopID();
        ImGui::TableSetColumnIndex(2);
        ToggleVisibility(sceneMesh);
      }
    }
    ImGui::EndTable();
  }
  ImGui::PopStyleColor();
  ImGui::PopStyleColor();
  ImGui::PopStyleColor();
  ImGui::End();
}


/* -----------------------------------------------------
 *          PRIVATE METHODS
 * -----------------------------------------------------
*/

void OutlinerPanel::AddSceneComponentButton(const char* labelPopup)
{
  ImGui::PushStyleColor(ImGuiCol_Button, { 1.f,1.f,1.f,1.f });
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.75f,0.75f,0.75f,1.f });
  ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.5f,0.5f,0.5f,1.f });

  if (ImGui::ImageButton((ImTextureID)_icons[PLUS]->textureID, { 24,24 }))
    ImGui::OpenPopup(labelPopup);

  ImGui::InvisibleButton("##margin-bottom", { 8,8 });
  ImGui::Separator();
  ImGui::InvisibleButton("##margin-bottom", { 8,8 });

  ImGui::PopStyleColor(3);
}

void OutlinerPanel::AddSceneComponentPopup(Scene* scene)
{
  const char* lights[] = { "Directional light", "Point light", "Spot light", };
  const char* meshes[] = { "Cube", "Plane", "Cylinder", };
  ImGui::SeparatorText("New light");
  for (int i = 0; i < IM_ARRAYSIZE(lights); i++)
  {
    if (ImGui::Selectable(lights[i]))
    {
      switch (i)
      {
      case 0: /* Directional lihght */
      {
        if (!scene->HasDirLight())
        {
          auto obj = std::make_shared<DirectionalLight>("UDirLight");
          scene->AddSceneObject(obj);
        }
        break;
      }
      case 1: /* Pointlight */
      {
        if (scene->scenePLights.size() < 4)
        {
          char uname[32]{};
          sprintf_s(uname, "UPointLight[%d]", (int)scene->scenePLights.size());
          auto obj = std::make_shared<PointLight>(uname);
          scene->AddSceneObject(obj);
          break;
        }
      }
      case 2: /* TODO: Spotlight */
      {
        break;
      }
      }
    }
  }

  ImGui::SeparatorText("New static mesh");
  for (int i = 0; i < IM_ARRAYSIZE(meshes); i++)
  {
    Path model;
    if (ImGui::Selectable(meshes[i]))
    {
      switch (i)
      {
      case 0: /* Cube mesh */
      {
        model = "Assets/Shapes/Cube/Cube.obj";
        model = model.lexically_normal();
        auto obj = std::make_shared<StaticMesh>(ROOT_PATH / model);
        scene->AddSceneObject(obj);
        break;
      }
      case 1: /* Plane mesh */
      {
        model = "Assets/Shapes/Plane/Plane.obj";
        model = model.lexically_normal();
        auto obj = std::make_shared<StaticMesh>(ROOT_PATH / model);
        scene->AddSceneObject(obj);
        break;
      }
      case 2: /* Cylinder mesh */
      {
        model = "Assets/Shapes/Cylinder/Cylinder.obj";
        model = model.lexically_normal();
        auto obj = std::make_shared<StaticMesh>(ROOT_PATH / model);
        scene->AddSceneObject(obj);
        break;
      }
      }
    }
  }
  ImGui::EndPopup();
}

void OutlinerPanel::ToggleVisibility(SharedPointer<DirectionalLight>& dLight)
{
  if (dLight->visible)
  {
    if (ImGui::ImageButtonEx(_buttonEyeID++, (ImTextureID)_icons[EYE]->textureID, { _iconSize,_iconSize }, { 0,0 }, { 1,1 }, { 0,0,0,0 }, { 1,1,1,1 }))
      dLight->visible = false;
  }
  else
  {
    if (ImGui::ImageButtonEx(_buttonEyeID++, (ImTextureID)_icons[EYE_HIDDEN]->textureID, { _iconSize,_iconSize }, { 0,0 }, { 1,1 }, { 0,0,0,0 }, { 1,1,1,1 }))
      dLight->visible = true;
  }
  
  //auto shader = ShadersManager::Instance().GetShader("SceneShader");
  //shader->SetBool("UDirLightVisible", dLight->visible);
}

void OutlinerPanel::ToggleVisibility(SharedPointer<PointLight>& pLight)
{
  if (pLight->visible)
  {
    if (ImGui::ImageButtonEx(_buttonEyeID++, (ImTextureID)_icons[EYE]->textureID, { _iconSize,_iconSize }, { 0,0 }, { 1,1 }, { 0,0,0,0 }, { 1,1,1,1 }))
      pLight->visible = false;
  }
  else
  {
    if (ImGui::ImageButtonEx(_buttonEyeID++, (ImTextureID)_icons[EYE_HIDDEN]->textureID, { _iconSize,_iconSize }, { 0,0 }, { 1,1 }, { 0,0,0,0 }, { 1,1,1,1 }))
      pLight->visible = true;
  }

  //char uniform[32];
  //sprintf_s(uniform, "UPointLightVisible[%d]", sceneObj->GetID());
  //auto shader = ShadersManager::Instance().GetShader("SceneShader");
  //shader->SetInt(uniform, (int) sceneObj->visible);
}

void OutlinerPanel::ToggleVisibility(SharedPointer<StaticMesh>& sMesh)
{
  if (sMesh->visible)
  {
    if (ImGui::ImageButtonEx(_buttonEyeID++, (ImTextureID)_icons[EYE]->textureID, { _iconSize,_iconSize }, { 0,0 }, { 1,1 }, { 0,0,0,0 }, { 1,1,1,1 }))
      sMesh->visible = false;
  }
  else
  {
    if (ImGui::ImageButtonEx(_buttonEyeID++, (ImTextureID)_icons[EYE_HIDDEN]->textureID, { _iconSize,_iconSize }, { 0,0 }, { 1,1 }, { 0,0,0,0 }, { 1,1,1,1 }))
      sMesh->visible = true;
  }
}

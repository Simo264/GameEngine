#include "ScenePanel.hh"


#include "../Logger.hh"
#include "../Subsystems/TexturesManager.hh"

#include "Imgui/imgui.h"
#include "Imgui/imgui_impl_glfw.h"
#include "Imgui/imgui_impl_opengl3.h"
#include "Imgui/imgui_stdlib.h"
#include "Imgui/imgui_internal.h"
#include "Imgui/imgui_spectrum.h"

/* -----------------------------------------------------
 *          PUBLIC METHODS
 * -----------------------------------------------------
*/

ScenePanel::ScenePanel(const char* panelName, Vec2i panelSize)
{
  isOpen = true;
  _panelName = panelName;
  _panelSize = panelSize;
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

void ScenePanel::RenderPanel(Scene* scene)
{
  if (!isOpen)
    return;

  ImGui::Begin(_panelName.c_str(), &isOpen);

  //AddSceneComponentButton("PopupAddElement");
  //if (ImGui::BeginPopup("PopupAddElement"))
  //  AddSceneComponentPopup(scene);

  ImGui::PushStyleColor(ImGuiCol_Button, { 0,0,0,0 });
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.25f,0.25f,0.25f, 0.75f });
  ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.25f,0.25f,0.25f, 0.75f });
  if (ImGui::BeginTable("Table", 3, ImGuiTableFlags_RowBg))
  {
    ImGui::TableSetupColumn("C1", ImGuiTableColumnFlags_WidthFixed, 0.1f * ImGui::GetContentRegionAvail().x);   // 10% width
    ImGui::TableSetupColumn("C2", ImGuiTableColumnFlags_WidthFixed, 0.7f * ImGui::GetContentRegionAvail().x);   // 70% width
    ImGui::TableSetupColumn("C3", ImGuiTableColumnFlags_WidthFixed, 0.2f * ImGui::GetContentRegionAvail().x);   // 20% width

    _buttonEyeID = 1;

    /* Directional light row */
    if (scene->HasDirLight())
    {
      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Image((ImTextureID)_icons[SUN]->textureID, { _iconSize,_iconSize });
      ImGui::TableSetColumnIndex(1);
      if (ImGui::Selectable(scene->sceneDLight->name.c_str(), (_dLightSelected != nullptr)))
      {
        _dLightSelected = scene->sceneDLight;
        _pLightSelected = nullptr;
        _sMeshSelected = nullptr;
      }
      ImGui::TableSetColumnIndex(2);
      ToggleVisibility(scene->sceneDLight->visible);
    }

    /* Point light rows */
    if (scene->HasPointLights())
    {
      for (auto scenePLight : scene->scenePLights)
      {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Image((ImTextureID)_icons[LAMP]->textureID, {_iconSize,_iconSize});
        ImGui::TableSetColumnIndex(1);
        if (ImGui::Selectable(scenePLight->name.c_str(), (_pLightSelected == scenePLight)))
        {
          _dLightSelected = nullptr;
          _pLightSelected = scenePLight;
          _sMeshSelected = nullptr;
        }
        
        ImGui::TableSetColumnIndex(2);
        ToggleVisibility(scenePLight->visible);
      }
    }

    /* Static mesh rows */
    if (scene->HasStaticMeshes())
    {
      for (auto sceneMesh : scene->sceneSMeshes)
      {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Image((ImTextureID)_icons[MESH]->textureID, { _iconSize,_iconSize });
        ImGui::TableSetColumnIndex(1);
        if (ImGui::Selectable(sceneMesh->name.c_str(), (_sMeshSelected == sceneMesh)))
        {
          _dLightSelected = nullptr;
          _pLightSelected = nullptr;
          _sMeshSelected = sceneMesh;
        }
        ImGui::TableSetColumnIndex(2);
        ToggleVisibility(sceneMesh->visible);
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

void ScenePanel::ToggleVisibility(bool& b)
{
  /* Toggle visibility */
  if (b)
  {
    if (ImGui::ImageButtonEx(_buttonEyeID++, (ImTextureID)_icons[EYE]->textureID, {_iconSize,_iconSize}, {0,0}, {1,1}, {0,0,0,0}, {1,1,1,1}))
      b = false;
  }
  else
  {
    if (ImGui::ImageButtonEx(_buttonEyeID++, (ImTextureID)_icons[EYE_HIDDEN]->textureID, { _iconSize,_iconSize }, { 0,0 }, { 1,1 }, { 0,0,0,0 }, { 1,1,1,1 }))
      b = true;
  }
}


#if 0
void ScenePanel::AddSceneComponentButton(const char* labelPopup)
{
  ImGui::PushStyleColor(ImGuiCol_Button, { 1.f,1.f,1.f,1.f });
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.75f,0.75f,0.75f,1.f });
  ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.5f,0.5f,0.5f,1.f });

  if (ImGui::ImageButton((ImTextureID)_iconPlus->textureID, { 24,24 }))
    ImGui::OpenPopup(labelPopup);

  ImGui::InvisibleButton("##margin-bottom", { 8,8 });
  ImGui::Separator();
  ImGui::InvisibleButton("##margin-bottom", { 8,8 });

  ImGui::PopStyleColor();
  ImGui::PopStyleColor();
  ImGui::PopStyleColor();
}

void ScenePanel::AddSceneComponentPopup(Scene* scene)
{
  const char* lights[] = { "Directional light", "Point light", "Spot light", };
  const char* meshes[] = { "Cube", "Plane", "Cylinder", };
  char buff[32]{};

  ImGui::SeparatorText("Add lighting");
  for (int i = 0; i < IM_ARRAYSIZE(lights); i++)
  {
    if (ImGui::Selectable(lights[i]))
    {
      if (i == 0) /* Directional light*/
      {
        if (!scene->HasDirLight())
        {
          auto dirLight = new DirectionalLight("UDirLight");
          auto sceneDirLight = SceneObject<DirectionalLight>::Create(dirLight, true, "Directional light");
          scene->AddDirectionalLight(*sceneDirLight);
        }
      }
      else if (i == 1) /* Point light */
      {
        //sprintf_s(buff, "UPointLight[%d]", scene->pLights.size());
        //scene->pLights.push_back({ new PointLight(buff), true } );
      }
      else if (i == 2) /* TODO: Spot light */
      {
        CONSOLE_TRACE("ADD SPOT LIGHT...");
      }
    }
  }

  ImGui::SeparatorText("Add static mesh");
  for (int i = 0; i < IM_ARRAYSIZE(meshes); i++)
  {
    if (ImGui::Selectable(meshes[i]))
    {
      switch (i)
      {
        /* Cube mesh */
      case 0:
        //scene->AddStaticMesh( new StaticMesh(ROOT_PATH / "Assets/Shapes/Cube/Cube.obj"));
        break;
        /* Plane mesh */
      case 1:
        //scene->AddStaticMesh(new StaticMesh(ROOT_PATH / "Assets/Shapes/Plane/Plane.obj"));
        break;
        /* Cylinder mesh */
      case 2:
        //scene->AddStaticMesh( new StaticMesh(ROOT_PATH / "Assets/Shapes/Cylinder/Cylinder.obj"));
        break;

      default:
        break;
      }
    }
  }
  ImGui::EndPopup();
}

#endif

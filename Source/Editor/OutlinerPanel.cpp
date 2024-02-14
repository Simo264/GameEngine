#include "OutlinerPanel.hh"


#include "../Scene.hh"

#include "../Mesh/StaticMesh.hh"
#include "../Lighting/DirectionalLight.hh"
#include "../Lighting/PointLight.hh"
#include "../Lighting/SpotLight.hh"

#include "../Shader.hh"
#include "../Logger.hh"
#include "../Subsystems/TexturesManager.hh"
#include "../Subsystems/ShadersManager.hh"

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
      if(ImGui::Selectable(scene->sceneDLight->tagName.c_str(), (_dLightSelected != nullptr)))
      {
        _dLightSelected = scene->sceneDLight;
        _pLightSelected = nullptr;
        _sMeshSelected = nullptr;
      }
      ImGui::PopID();
      ImGui::TableSetColumnIndex(2);
      ToggleVisibility(static_cast<DirectionalLight*>(scene->sceneDLight));
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
        ImGui::PushID(scenePLight->GetID());
        if (ImGui::Selectable(scenePLight->tagName.c_str(), (_pLightSelected == scenePLight)))
        {
          _dLightSelected = nullptr;
          _pLightSelected = scenePLight;
          _sMeshSelected = nullptr;
        }
        ImGui::PopID();
        ImGui::TableSetColumnIndex(2);
        ToggleVisibility(static_cast<PointLight*>(scenePLight));
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
        ImGui::PushID(sceneMesh->GetID());
        if (ImGui::Selectable(sceneMesh->tagName.c_str(), (_sMeshSelected == sceneMesh)))
        {
          _dLightSelected = nullptr;
          _pLightSelected = nullptr;
          _sMeshSelected = sceneMesh;
        }
        ImGui::PopID();
        ImGui::TableSetColumnIndex(2);
        ToggleVisibility(static_cast<StaticMesh*>(sceneMesh));
      }
    }
    ImGui::EndTable();
  }
  ImGui::PopStyleColor();
  ImGui::PopStyleColor();
  ImGui::PopStyleColor();
  ImGui::End();
}

template<>
SceneObject* OutlinerPanel::GetItemSelected<DirectionalLight>() { return _dLightSelected; }

template<>
SceneObject* OutlinerPanel::GetItemSelected<PointLight>() { return _pLightSelected; }

template<>
SceneObject* OutlinerPanel::GetItemSelected<StaticMesh>() { return _sMeshSelected; }

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
  char buff[32]{};

  ImGui::SeparatorText("New light");
  for (int i = 0; i < IM_ARRAYSIZE(lights); i++)
  {
    if (ImGui::Selectable(lights[i]))
    {
      if (i == 0) /* Directional light*/
      {
        if (!scene->HasDirLight())
        {
          CONSOLE_TRACE("ADD DIR LIGHT...");
          //auto dirLight = new DirectionalLight("UDirLight");
          //auto sceneDirLight = SceneObject<DirectionalLight>::Create(dirLight, true, "Directional light");
          //scene->AddDirectionalLight(*sceneDirLight);
        }
        else
        {
          CONSOLE_TRACE("DIR LIGHT ALREADY EXISTS");
        }
      }
      else if (i == 1) /* Point light */
      {
        CONSOLE_TRACE("ADD POINT LIGHT...");
        //sprintf_s(buff, "UPointLight[%d]", scene->pLights.size());
        //scene->pLights.push_back({ new PointLight(buff), true } );
      }
      else if (i == 2) /* TODO: Spot light */
      {
        CONSOLE_TRACE("ADD SPOT LIGHT...");
      }
    }
  }

  ImGui::SeparatorText("New static mesh");
  for (int i = 0; i < IM_ARRAYSIZE(meshes); i++)
  {
    if (ImGui::Selectable(meshes[i]))
    {
      switch (i)
      {
        /* Cube mesh */
      case 0:
        CONSOLE_TRACE("ADD CUBE MESH...");
        //scene->AddStaticMesh( new StaticMesh(ROOT_PATH / "Assets/Shapes/Cube/Cube.obj"));
        break;
        /* Plane mesh */
      case 1:
        CONSOLE_TRACE("ADD PLANE MESH...");
        //scene->AddStaticMesh(new StaticMesh(ROOT_PATH / "Assets/Shapes/Plane/Plane.obj"));
        break;
        /* Cylinder mesh */
      case 2:
        CONSOLE_TRACE("ADD CYLINDER MESH...");
        //scene->AddStaticMesh( new StaticMesh(ROOT_PATH / "Assets/Shapes/Cylinder/Cylinder.obj"));
        break;

      default:
        break;
      }
    }
  }
  ImGui::EndPopup();
}


/* Toggle dir light visibility */
void OutlinerPanel::ToggleVisibility(DirectionalLight* dLight)
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
  
  auto shader = ShadersManager::Instance().GetShader("SceneShader");
  shader->SetBool("UDirLightVisible", dLight->visible);
}

/* Toggle point light visibility */
void OutlinerPanel::ToggleVisibility(PointLight* pLight)
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

/* Toggle static mesh visibility */
void OutlinerPanel::ToggleVisibility(StaticMesh* sMesh)
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

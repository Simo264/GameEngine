#include "ScenePanel.hh"
#include "../Mesh/StaticMesh.hh"
#include "../Lighting/DirectionalLight.hh"
#include "../Lighting/PointLight.hh"
#include "../Lighting/SpotLight.hh"
#include "../Scene.hh"
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
  isPropertiesOpen = false;

  _panelName = panelName;
  _panelSize = panelSize;

  _dirLightSelected = false;
  _pointLightSelected = -1;
  _staticMeshSelected = -1;

  auto& instanceTM = TexturesManager::Instance();
  _iconSun  = instanceTM.GetTextureByPath(ROOT_PATH / "Icons/icon-sun.png");
  _iconLamp = instanceTM.GetTextureByPath(ROOT_PATH / "Icons/icon-lamp.png");
  _iconEye = instanceTM.GetTextureByPath(ROOT_PATH / "Icons/icon-eye.png");
  _iconEyeHidden = instanceTM.GetTextureByPath(ROOT_PATH / "Icons/icon-eye-hidden.png");
  _iconPlus = instanceTM.GetTextureByPath(ROOT_PATH / "Icons/icon-plus.png");
}

void ScenePanel::RenderPanel(Scene* scene)
{
  if (!isOpen)
    return;

  ImGui::Begin(_panelName.c_str(), &isOpen);

  AddSceneComponentButton("PopupAddElement");
  if (ImGui::BeginPopup("PopupAddElement"))
    AddSceneComponentPopup(scene);


  ImGui::PushStyleColor(ImGuiCol_Button, { 0,0,0,0 });
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.25f,0.25f,0.25f, 0.75f });
  ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.25f,0.25f,0.25f, 0.75f });
  if (ImGui::BeginTable("Table", 3, ImGuiTableFlags_RowBg))
  {
    ImGui::TableSetupColumn("C1", ImGuiTableColumnFlags_WidthFixed, 0.1f * ImGui::GetContentRegionAvail().x);   // 10% width
    ImGui::TableSetupColumn("C2", ImGuiTableColumnFlags_WidthFixed, 0.7f * ImGui::GetContentRegionAvail().x);   // 70% width
    ImGui::TableSetupColumn("C3", ImGuiTableColumnFlags_WidthFixed, 0.2f * ImGui::GetContentRegionAvail().x);   // 20% width

    /* Directiona light row */
    if (scene->dLight.object)
      DirLightRow("Directional_light");

    /* Point light rows */
    for (int i = 0; i < scene->pLights.size(); i++)
    {
      char label[32];
      sprintf_s(label, "Point_light_%d", i + 1);
      PointLightRow(label, i);
    }

    /* Static mesh rows */
    for (int i = 0; i < scene->sMeshes.size(); i++)
    {
      char label[32];
      sprintf_s(label, "Static_mesh_%d", i + 1);
      //PointLightRow(label, i);
    }

    ImGui::EndTable();
  }
  ImGui::PopStyleColor();
  ImGui::PopStyleColor();
  ImGui::PopStyleColor();

  ImGui::End();

  if (isPropertiesOpen && _dirLightSelected)
    ShowPropertiesPanel(scene->dLight.object);
  else if (isPropertiesOpen && _pointLightSelected >= 0)
    ShowPropertiesPanel(scene->pLights.at(_pointLightSelected).object);
  else if (isPropertiesOpen && _staticMeshSelected >= 0)
    ShowPropertiesPanel(scene->sMeshes.at(_staticMeshSelected).object);
}

/* -----------------------------------------------------
 *          PRIVATE METHODS
 * -----------------------------------------------------
*/

void ScenePanel::DirLightRow(const char* label)
{
  ImGui::TableNextRow();
  
  ImGui::TableSetColumnIndex(0);
  ImGui::Image((ImTextureID)_iconSun->textureID, { 16,16 });

  ImGui::TableSetColumnIndex(1);
  if (ImGui::Selectable(label, _dirLightSelected == true))
  {
    isPropertiesOpen = true;
    _dirLightSelected = true;
    _staticMeshSelected = -1;
    _pointLightSelected = -1;
  }

  ImGui::TableSetColumnIndex(2);
  ImGui::ImageButton((ImTextureID)_iconEye->textureID, { 16,16 });
}

void ScenePanel::PointLightRow(const char* label, int i)
{
  ImGui::TableNextRow();

  ImGui::TableSetColumnIndex(0);
  ImGui::Image((ImTextureID)_iconLamp->textureID, { 16,16 });

  ImGui::TableSetColumnIndex(1);
  if (ImGui::Selectable(label, (_pointLightSelected == i), ImGuiSelectableFlags_SpanAvailWidth))
  {
    isPropertiesOpen = true;
    _dirLightSelected = false;
    _pointLightSelected = i;
    _staticMeshSelected = -1;
  }
  
  ImGui::TableSetColumnIndex(2);
  ImGui::ImageButton((ImTextureID)_iconEye->textureID, { 16,16 });
}

void ScenePanel::StaticMeshRow(const char* label, int i)
{
  ImGui::TableNextRow();

  ImGui::TableSetColumnIndex(0);
  ImGui::Image((ImTextureID)_iconSun->textureID, { 16,16 });
  
  ImGui::TableSetColumnIndex(1);
  if (ImGui::Selectable(label, _staticMeshSelected == i))
  {
    isPropertiesOpen = true;
    _dirLightSelected = false;
    _pointLightSelected = -1;
    _staticMeshSelected = i;
  }

  ImGui::TableSetColumnIndex(2);
  ImGui::ImageButton((ImTextureID)_iconEye->textureID, { 16,16 });
}

void ScenePanel::ShowPropertiesPanel(StaticMesh* meshTarget)
{
  if (!isPropertiesOpen)
    return;
  
  /* Update texture vector */
  TexturesManager::Instance().CopyTextures(_textures);

  ImGui::Begin("Properties", &isPropertiesOpen);
  ImGui::Text("(StaticMesh name)");

  ImGui::SeparatorText("Transformation");
  ImGui::DragFloat3("Translation", (float*)&meshTarget->position, 0.1f, -FLT_MAX, +FLT_MAX);
  ImGui::DragFloat3("Scaling", (float*)&meshTarget->scaling, 0.1f, -FLT_MAX, +FLT_MAX);
  ImGui::DragFloat("Rotation angle", (float*)&meshTarget->rotationAngle, 0.5f, -180.0f, +180.0f);
  ImGui::SliderFloat3("Rotation axis", (float*)&meshTarget->rotationAxis, 0.0f, 1.0f);

  ImGui::SeparatorText("Material");

  const String diffusePathStr = (meshTarget->diffuse ? meshTarget->diffuse->texturePath.string() : "");
  const uint32_t diffuseID = (meshTarget->diffuse ? meshTarget->diffuse->textureID : 0);
  if (meshTarget->diffuse)
    ImGui::Text(diffusePathStr.c_str());
  else
    ImGui::Text("None");

  ImGui::Image(
    (ImTextureID)diffuseID,
    ImVec2(64.0f, 64.0f) /* image size */
  );

  if (ImGui::BeginCombo("Textures", diffusePathStr.c_str()))
  {
    for (int i = 0; i < _textures.size(); i++)
    {
      String textPathStr = _textures[i]->texturePath.string();
      bool isSelected = (std::strcmp(diffusePathStr.c_str(), textPathStr.c_str()) == 0);
      if (ImGui::Selectable(textPathStr.c_str(), isSelected))
        meshTarget->diffuse = TexturesManager::Instance().GetTextureByPath(textPathStr.c_str());
      if (isSelected)
        ImGui::SetItemDefaultFocus();
    }
    ImGui::EndCombo();
  }
  ImGui::End();
}

void ScenePanel::ShowPropertiesPanel(DirectionalLight* dirLight)
{
  if (!isPropertiesOpen)
    return;

  ImGui::Begin("Properties", &isPropertiesOpen);
  ImGui::SeparatorText("Directional light");

  ImGui::ColorEdit3("Color", (float*)&dirLight->color);
  ImGui::SliderFloat("Ambient", &dirLight->ambient, 0.0f, 1.0f);
  ImGui::SliderFloat("Diffuse", &dirLight->diffuse, 0.0f, 1.0f);
  ImGui::SliderFloat("Specular", &dirLight->specular, 0.0f, 1.0f);
  ImGui::DragFloat3("Direction", (float*)&dirLight->direction, 0.1f, -FLT_MAX, +FLT_MAX);

  ImGui::End();
}

void ScenePanel::ShowPropertiesPanel(PointLight* pointLight)
{
  if (!isPropertiesOpen)
    return;

  ImGui::Begin("Properties", &isPropertiesOpen);
  ImGui::SeparatorText("Point light");

  ImGui::ColorEdit3("Color", (float*)&pointLight->color);
  ImGui::SliderFloat("Ambient", &pointLight->ambient, 0.0f, 1.0f);
  ImGui::SliderFloat("Diffuse", &pointLight->diffuse, 0.0f, 1.0f);
  ImGui::SliderFloat("Specular", &pointLight->specular, 0.0f, 1.0f);
  ImGui::DragFloat3("Position", (float*)&pointLight->position, 0.1f, -FLT_MAX, +FLT_MAX);

  ImGui::End();
}

void ScenePanel::AddSceneComponentButton(const char* labelPopup)
{
  ImGui::PushStyleColor(ImGuiCol_Button, { 0.1f,0.1f,0.1f,0.75f });
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.75f,0.75f,0.75f,0.75f });
  ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.5f,0.5f,0.5f,0.75f });

  if (ImGui::Button("Add"))
    ImGui::OpenPopup(labelPopup);

  ImGui::PopStyleColor();
  ImGui::PopStyleColor();
  ImGui::PopStyleColor();
}

void ScenePanel::AddSceneComponentPopup(Scene* scene)
{
  const char* lights[] = { "Directional light", "Point light", "Spot light", };
  const char* meshes[] = { "Cube", "Plane", "Cylinder", };
  char buff[32];

  ImGui::SeparatorText("Add lighting");
  for (int i = 0; i < IM_ARRAYSIZE(lights); i++)
  {
    if (ImGui::Selectable(lights[i]))
    {
      if (i == 0) /* Directional light*/
      {
        if (!scene->dLight.object)
          scene->dLight = { new DirectionalLight("UDirLight"), true };
      }
      else if (i == 1) /* Point light */
      {
        sprintf_s(buff, "UPointLight[%d]", scene->pLights.size());
        scene->pLights.push_back({ new PointLight(buff), true } );
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
        scene->AddStaticMesh( new StaticMesh(ROOT_PATH / "Assets/Shapes/Cube/Cube.obj"));
        break;
        /* Plane mesh */
      case 1:
        scene->AddStaticMesh(new StaticMesh(ROOT_PATH / "Assets/Shapes/Plane/Plane.obj"));
        break;
        /* Cylinder mesh */
      case 2:
        scene->AddStaticMesh( new StaticMesh(ROOT_PATH / "Assets/Shapes/Cylinder/Cylinder.obj"));
        break;

      default:
        break;
      }
    }
  }
  ImGui::EndPopup();
}

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
  isPropertiesOpen = false;

  _panelName = panelName;
  _panelSize = panelSize;

  _dirLightSelected = false;
  _pointLightSelected = -1;
  _staticMeshSelected = -1;
  
  _iconSize = 16.0f;

  auto& instanceTM = TexturesManager::Instance();
  _iconSun  = instanceTM.GetTextureByPath(ROOT_PATH / "Icons/icon-sun.png");
  _iconLamp = instanceTM.GetTextureByPath(ROOT_PATH / "Icons/icon-lamp.png");
  _iconEye = instanceTM.GetTextureByPath(ROOT_PATH / "Icons/icon-eye.png");
  _iconEyeHidden = instanceTM.GetTextureByPath(ROOT_PATH / "Icons/icon-eye-hidden.png");
  _iconMesh = instanceTM.GetTextureByPath(ROOT_PATH / "Icons/icon-mesh.png");
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
    if (scene->HasDirLight())
      DirLightRow(scene->sceneDLight);

    /* Point light rows */
    if (scene->HasPointLights())
    {
      for (auto scenePLight : scene->scenePLights)
      {
        //  char label[32];
        //  sprintf_s(label, "Point_light_%d", i + 1);
        //  PointLightRow(label, i);
      }
    }

    /* Static mesh rows */
    if (scene->HasStaticMeshes())
    {
      for (auto sceneMesh : scene->sceneSMeshes)
      {
        //  char label[32];
        //  sprintf_s(label, "Static_mesh_%d", i + 1);
        //  StaticMeshRow(label, i);
      }
    }


    ImGui::EndTable();
  }
  ImGui::PopStyleColor();
  ImGui::PopStyleColor();
  ImGui::PopStyleColor();

  ImGui::End();

  if (isPropertiesOpen && _dirLightSelected)
    ShowPropertiesPanel(scene, scene->sceneDLight);
  //else if (isPropertiesOpen && _pointLightSelected >= 0)
  //  ShowPropertiesPanel(scene->pLights.at(_pointLightSelected).object);
  //else if (isPropertiesOpen && _staticMeshSelected >= 0)
  //  ShowPropertiesPanel(scene->sMeshes.at(_staticMeshSelected).object);
}

/* -----------------------------------------------------
 *          PRIVATE METHODS
 * -----------------------------------------------------
*/

void ScenePanel::DirLightRow(SceneObject<DirectionalLight>* sceneDirLight)
{
  ImGui::TableNextRow();
  
  ImGui::TableSetColumnIndex(0);
  ImGui::Image((ImTextureID)_iconSun->textureID, { _iconSize,_iconSize });

  ImGui::TableSetColumnIndex(1);
  if (ImGui::Selectable(sceneDirLight->name.c_str(), _dirLightSelected == true))
  {
    isPropertiesOpen = true;
    _dirLightSelected = true;
    _staticMeshSelected = -1;
    _pointLightSelected = -1;
  }

  ImGui::TableSetColumnIndex(2);
  if (sceneDirLight->visible)
  {
    if (ImGui::ImageButton((ImTextureID)_iconEye->textureID, { _iconSize,_iconSize }))
      sceneDirLight->visible = false;
  }
  else
  {
    if (ImGui::ImageButton((ImTextureID)_iconEyeHidden->textureID, { _iconSize,_iconSize }))
      sceneDirLight->visible = true;
  }
}

void ScenePanel::PointLightRow(const char* label, int i)
{
  ImGui::TableNextRow();

  ImGui::TableSetColumnIndex(0);
  ImGui::Image((ImTextureID)_iconLamp->textureID, { _iconSize,_iconSize });

  ImGui::TableSetColumnIndex(1);
  if (ImGui::Selectable(label, (_pointLightSelected == i), ImGuiSelectableFlags_SpanAvailWidth))
  {
    isPropertiesOpen = true;
    _dirLightSelected = false;
    _pointLightSelected = i;
    _staticMeshSelected = -1;
  }
  
  ImGui::TableSetColumnIndex(2);
  ImGui::ImageButton((ImTextureID)_iconEye->textureID, { _iconSize,_iconSize });
}

void ScenePanel::StaticMeshRow(const char* label, int i)
{
  ImGui::TableNextRow();

  ImGui::TableSetColumnIndex(0);
  ImGui::Image((ImTextureID)_iconMesh->textureID, { _iconSize,_iconSize });
  
  ImGui::TableSetColumnIndex(1);
  if (ImGui::Selectable(label, _staticMeshSelected == i))
  {
    isPropertiesOpen = true;
    _dirLightSelected = false;
    _pointLightSelected = -1;
    _staticMeshSelected = i;
  }

  ImGui::TableSetColumnIndex(2);
  ImGui::ImageButton((ImTextureID)_iconEye->textureID, { _iconSize,_iconSize });
}

void ScenePanel::ShowPropertiesPanel(Scene* scene, SceneObject<DirectionalLight>* sceneDirLight)
{
  if (!isPropertiesOpen)
    return;

  ImGui::Begin("Properties", &isPropertiesOpen);
  ImGui::SeparatorText(sceneDirLight->name.c_str());

  auto obj = sceneDirLight->object;
  ImGui::ColorEdit3("Color", (float*)&obj->color);
  ImGui::SliderFloat("Ambient", &obj->ambient, 0.0f, 1.0f);
  ImGui::SliderFloat("Diffuse", &obj->diffuse, 0.0f, 1.0f);
  ImGui::SliderFloat("Specular", &obj->specular, 0.0f, 1.0f);
  ImGui::DragFloat3("Direction", (float*)&obj->direction, 0.1f, -FLT_MAX, +FLT_MAX);

  //if (ImGui::Button("Delete"))
  //{
  //  scene->RemoveDirectionalLight();
  //  isPropertiesOpen = false;
  //}

  ImGui::End();
}

void ScenePanel::ShowPropertiesPanel(SceneObject<PointLight>* scenePointLight)
{
  if (!isPropertiesOpen)
    return;

  ImGui::Begin("Properties", &isPropertiesOpen);
  ImGui::SeparatorText(scenePointLight->name.c_str());

  auto obj = scenePointLight->object;
  ImGui::ColorEdit3("Color", (float*)&obj->color);
  ImGui::SliderFloat("Ambient", &obj->ambient, 0.0f, 1.0f);
  ImGui::SliderFloat("Diffuse", &obj->diffuse, 0.0f, 1.0f);
  ImGui::SliderFloat("Specular", &obj->specular, 0.0f, 1.0f);
  ImGui::DragFloat3("Position", (float*)&obj->position, 0.1f, -FLT_MAX, +FLT_MAX);

  ImGui::End();
}

void ScenePanel::ShowPropertiesPanel(SceneObject<StaticMesh>* sceneMesh)
{
  if (!isPropertiesOpen)
    return;
  
  /* Update texture vector */
  TexturesManager::Instance().CopyTextures(_textures);

  ImGui::Begin("Properties", &isPropertiesOpen);
  ImGui::Text(sceneMesh->name.c_str());

  auto obj = sceneMesh->object;
  ImGui::SeparatorText("Transformation");
  ImGui::DragFloat3("Translation", (float*)&obj->position, 0.1f, -FLT_MAX, +FLT_MAX);
  ImGui::DragFloat3("Scaling", (float*)&obj->scaling, 0.1f, -FLT_MAX, +FLT_MAX);
  ImGui::DragFloat("Rotation angle", (float*)&obj->rotationAngle, 0.5f, -180.0f, +180.0f);
  ImGui::SliderFloat3("Rotation axis", (float*)&obj->rotationAxis, 0.0f, 1.0f);

  ImGui::SeparatorText("Material");

  const String diffusePathStr = (obj->diffuse ? obj->diffuse->texturePath.string() : "");
  const uint32_t diffuseID = (obj->diffuse ? obj->diffuse->textureID : 0);
  if (obj->diffuse)
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
        obj->diffuse = TexturesManager::Instance().GetTextureByPath(textPathStr.c_str());
      if (isSelected)
        ImGui::SetItemDefaultFocus();
    }
    ImGui::EndCombo();
  }
  ImGui::End();
}

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

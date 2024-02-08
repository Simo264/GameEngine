#include "PropertyPanel.hh"

#include "../Lighting/DirectionalLight.hh"
#include "../Lighting/PointLight.hh"
#include "../Lighting/SpotLight.hh"
#include "../Mesh/StaticMesh.hh"

#include "../Subsystems/TexturesManager.hh"

#include "Imgui/imgui.h"
#include "Imgui/imgui_impl_glfw.h"
#include "Imgui/imgui_impl_opengl3.h"
#include "Imgui/imgui_stdlib.h"
#include "Imgui/imgui_internal.h"
#include "Imgui/imgui_spectrum.h"

PropertyPanel::PropertyPanel(const char* panelName, Vec2i panelSize)
{
	_panelName = panelName;
	_panelSize = panelSize;
}

void PropertyPanel::RenderPanel(SceneObject<DirectionalLight>& sceneDirLight)
{
	if (!isOpen)
		return;

  ImGui::Begin(_panelName.c_str(), &isOpen);
  ImGui::SeparatorText(sceneDirLight.name.c_str());

  auto obj = sceneDirLight.object;
  ImGui::ColorEdit3("Color", (float*)&obj->color);
  ImGui::SliderFloat("Ambient", &obj->ambient, 0.0f, 1.0f);
  ImGui::SliderFloat("Diffuse", &obj->diffuse, 0.0f, 1.0f);
  ImGui::SliderFloat("Specular", &obj->specular, 0.0f, 1.0f);
  ImGui::DragFloat3("Direction", (float*)&obj->direction, 0.1f, -FLT_MAX, +FLT_MAX);
  ImGui::End();
}

void PropertyPanel::RenderPanel(SceneObject<PointLight>& scenePointLight)
{
  if (!isOpen)
    return;

  ImGui::Begin("Properties", &isOpen);
  ImGui::SeparatorText(scenePointLight.name.c_str());

  auto obj = scenePointLight.object;
  ImGui::ColorEdit3("Color", (float*)&obj->color);
  ImGui::SliderFloat("Ambient", &obj->ambient, 0.0f, 1.0f);
  ImGui::SliderFloat("Diffuse", &obj->diffuse, 0.0f, 1.0f);
  ImGui::SliderFloat("Specular", &obj->specular, 0.0f, 1.0f);
  ImGui::DragFloat3("Position", (float*)&obj->position, 0.1f, -FLT_MAX, +FLT_MAX);
  ImGui::End();
}

void PropertyPanel::RenderPanel(SceneObject<StaticMesh>& sceneStaticMesh)
{
  if (!isOpen)
    return;

  /* Update texture vector */
  TexturesManager::Instance().CopyTextures(_textures);

  ImGui::Begin("Properties", &isOpen);
  ImGui::Text(sceneStaticMesh.name.c_str());

  auto obj = sceneStaticMesh.object;
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

  ImGui::Image((ImTextureID)diffuseID, { 64,64 } );

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

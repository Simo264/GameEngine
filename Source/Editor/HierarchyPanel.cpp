#include "HierarchyPanel.hh"
#include "../Mesh/StaticMesh.hh"
#include "../Lighting/DirectionalLight.hh"
#include "../Lighting/PointLight.hh"
#include "../Lighting/SpotLight.hh"
#include "../Scene.hh"

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

HierarchyPanel::HierarchyPanel(const char* name, Vec2i size)
{
  isOpen = true;
  isPropertiesOpen = false;

  panelName = name;
  panelSize = size;

  _dirLightSelected = false;
  _pointLightSelected = -1;
  _staticMeshSelected = -1;

  _treeName.reserve(32);
  _treeNode.reserve(32);
}

void HierarchyPanel::RenderPanel(Scene* scene)
{
  if (!isOpen)
    return;

  DirectionalLight*& dirLight = scene->directionalLight;
  Vector<PointLight*>& pointLights = scene->pointLights;
  Vector<StaticMesh*>& sceneMeshes = scene->statMeshes;
  
  ImGui::SetNextWindowSize(ImVec2(panelSize.x, panelSize.y));
  ImGui::Begin(panelName.c_str(), &isOpen);

  //if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
  //  ImGui::OpenPopup("my_select_popup");
  //
  //if (ImGui::BeginPopup("my_select_popup"))
  //{
  //  const char* lights[] = { 
  //    "Directional light", 
  //    "Point light", 
  //    "Spot light", 
  //  };
  //  const char* meshes[] = {
  //    "Cube",
  //    "Plane",
  //    "Cylinder",
  //  };
  //  
  //  ImGui::SeparatorText("Add lighting");
  //  for (int i = 0; i < IM_ARRAYSIZE(lights); i++)
  //  {
  //    if (ImGui::Selectable(lights[i]))
  //    {
  //      if (i == 0) /* Directional light*/
  //      {
  //        dirLight = new DirectionalLight("UDirLight");
  //      }
  //      else if (i == 1) /* Point light */
  //      {
  //        _itoa_s(pointLights.size(), digits, 10);
  //        String uniformname;
  //        uniformname.append("UPointLight[").append(digits).append("]");
  //        pointLights.push_back(new PointLight(uniformname.c_str()));
  //      }
  //      else if (i == 2) /* Spot light */
  //      {

  //      }
  //    }
  //  }
  //  ImGui::SeparatorText("Add static mesh");
  //  for (int i = 0; i < IM_ARRAYSIZE(meshes); i++)
  //  {
  //    if (ImGui::Selectable(meshes[i]))
  //    {
  //      if (i == 0) /* Cube mesh */
  //      {
  //      }
  //      else if (i == 1) /* Plane mesh */
  //      {
  //      }
  //      else if (i == 2) /* Cylinder mesh */
  //      {
  //      }
  //    }
  //  }
  //  ImGui::EndPopup();
  //}

  /* Lighting Tree */
  SetTreeName("Lighting");
  if (ImGui::TreeNode(_treeName.c_str()))
  {
    /* Directional light node */
    SetTreeNodeName("Directional light");
    if (dirLight)
      DirLightNode();

    /* Point light nodes */
    SetTreeName("Point light", pointLights.size());
    if (ImGui::TreeNode(_treeName.c_str()))
      PointLightNodes(pointLights);
    
    ImGui::TreePop();
  }

  /* Static mesh Tree */
  SetTreeName("Static mesh", sceneMeshes.size());
  if (ImGui::TreeNode(_treeName.c_str()))
  {
    /* Static mesh nodes */
    StaticMeshNodes(sceneMeshes);

    ImGui::TreePop();
  }
  ImGui::End();

  if (isPropertiesOpen && _dirLightSelected)
    ShowPropertiesPanel(dirLight);
  else if (isPropertiesOpen && _pointLightSelected >= 0)
    ShowPropertiesPanel(pointLights[_pointLightSelected]);
  else if (isPropertiesOpen && _staticMeshSelected >= 0)
    ShowPropertiesPanel(sceneMeshes[_staticMeshSelected]);
}

/* -----------------------------------------------------
 *          PRIVATE METHODS
 * -----------------------------------------------------
*/

void HierarchyPanel::SetTreeName(const char* name, int count)
{
  _treeName.clear();
  _treeName.append(name);
  
  if (count >= 0)
  {
    char digits[4]{};
    _itoa_s(count, digits, 10);

    _treeName.append("(");
    _treeName.append(digits);
    _treeName.append(")");
  }
}

void HierarchyPanel::SetTreeNodeName(const char* name, int count)
{
  _treeNode.clear();
  _treeNode.append(name);

  if(count >= 0)
  {
    char digits[4]{};
    _itoa_s(count, digits, 10);
    _treeNode.append("(");
    _treeNode.append(digits);
    _treeNode.append(")");
  }
}

void HierarchyPanel::DirLightNode()
{
  if (ImGui::Selectable(_treeNode.c_str(), _dirLightSelected == true))
  {
    isPropertiesOpen = true;
    _dirLightSelected = true;
    _staticMeshSelected = -1;
    _pointLightSelected = -1;
  }
}

void HierarchyPanel::PointLightNodes(Vector<PointLight*> pointLights)
{
  for (int i = 0; i < pointLights.size(); i++)
  {
    SetTreeNodeName("Light", i+1);
    if (ImGui::Selectable(_treeNode.c_str(), _pointLightSelected == i))
    {
      isPropertiesOpen = true;
      _dirLightSelected = false;
      _pointLightSelected = i;
      _staticMeshSelected = -1;
    }
  }
  ImGui::TreePop();
}

void HierarchyPanel::StaticMeshNodes(Vector<StaticMesh*> meshList)
{
  char digits[4]{};
  for (int i = 0; i < meshList.size(); i++)
  {
    _itoa_s(i, digits, 10);
    _treeNode.clear();
    _treeNode.append("StaticMesh").append(digits);
    if (ImGui::Selectable(_treeNode.c_str(), _staticMeshSelected == i))
    {
      isPropertiesOpen = true;
      _dirLightSelected = false;
      _pointLightSelected = -1;
      _staticMeshSelected = i;
    }
  }
}

void HierarchyPanel::ShowPropertiesPanel(StaticMesh* meshTarget)
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

void HierarchyPanel::ShowPropertiesPanel(DirectionalLight* dirLight)
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

void HierarchyPanel::ShowPropertiesPanel(PointLight* pointLight)
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
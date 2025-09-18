#include "Hierarchy.hpp"

#include "Core/Log/Logger.hpp"
#include "Core/Paths/Paths.hpp"
#include "Core/Dialog/FileDialog.hpp"

#include "Engine/Scene.hpp"
#include "Engine/Managers/TexturesManager.hpp"
#include "Engine/Managers/StaticMeshFactory.hpp"

#include <imgui.h>

using namespace Components;

enum class ObjectType : i32
{
  None = -1,
  StaticMesh,
  LightSource
};

struct NewEntityData
{
  Array<char, 32> tag{ "object" };
  bool tagIsValid = false;
  ObjectType objectType = ObjectType::None;
  LightType lightType = LightType::None;
  fs::path modelPath{};
};
static auto entityData = NewEntityData{};



// ----------------------------------------------------
//          PRIVATE
// ----------------------------------------------------

static bool Hierarchy_ButtonCentered(StringView label, ImVec2 size)
{
  auto avail = ImGui::GetContentRegionAvail().x;
  auto off = (avail - size.x) * 0.5f;
  if (off > 0.0f)
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);

  return ImGui::Button(label.data(), size);
}
static void Hierarchy_ListObjects(Scene& scene, GameObject& objSelected)
{
#if 0
  auto& texManager = TexturesManager::GetInstance();
  static auto icon = texManager.GetOrCreateIcon("game-object-16.png");

  auto selectableName = Array<char, 64>{};
  for (auto [entity, tag] : scene.GetEntityRegistry().view<Tag>().each())
  {
    auto archetypeId = ...;
    auto o = GameObject{ entity, &scene.GetEntityRegistry(), archetypeId };

    selectableName.fill(0);
    std::format_to_n(selectableName.data(), 
                     selectableName.size(), 
                     "{}##{}", 
                     tag.value.data(), 
                     static_cast<u32>(entity));

    ImGui::BeginGroup();
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + (ImGui::GetTextLineHeight() - 16.f) / 2);

    ImGui::Image(icon.id, ImVec2(16.f, 16.f));
    ImGui::SameLine();

    auto& colors = ImGui::GetStyle().Colors;
    auto colorSelected = colors[ImGuiCol_FrameBgActive];
    auto colorHovered = colors[ImGuiCol_FrameBgHovered];
    ImGui::PushStyleColor(ImGuiCol_Header, colorSelected);
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, colorHovered);
    ImGui::PushStyleColor(ImGuiCol_HeaderActive, colorSelected);

    auto selected = ImGui::Selectable(selectableName.data(), objSelected.Compare(o));
    if (selected && !objSelected.Compare(o))
      objSelected = o;

    if (ImGui::IsItemHovered() &&
        ImGui::IsMouseClicked(ImGuiMouseButton_Right) &&
        objSelected.IsValid())
      ImGui::OpenPopup("ObjectMenu");

    ImGui::PopStyleColor(3);
    ImGui::EndGroup();

    ImGui::Spacing();
  }

  if (ImGui::BeginPopup("ObjectMenu")) // Display menu on right click object
  {
    if (ImGui::MenuItem("Delete object"))
      scene.DestroyObject(objSelected.GetID());
    ImGui::EndPopup();
  }
#endif
}
static void Hierarchy_NewObjectModal(Scene& scene)
{
#if 0
  // Sezione per il Tag
  ImGui::BeginGroup();
  ImGui::Text("Tag");
  ImGui::InputText("##Tag", entityData.tag.data(), entityData.tag.size());
  entityData.tagIsValid = strlen(entityData.tag.data()) > 0;
  if (!entityData.tagIsValid)
  {
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Cannot be empty!");
  }
  ImGui::EndGroup();

  ImGui::Spacing();
  ImGui::Separator();
  ImGui::Spacing();

  ImGui::BeginGroup();
  ImGui::Text("Object Type");
  ImGui::RadioButton("Static Mesh", reinterpret_cast<i32*>(&entityData.objectType), static_cast<i32>(ObjectType::StaticMesh));
  ImGui::SameLine();
  ImGui::RadioButton("Light Source", reinterpret_cast<i32*>(&entityData.objectType), static_cast<i32>(ObjectType::LightSource));
  ImGui::EndGroup();

  if (entityData.objectType == ObjectType::LightSource)
  {
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::BeginGroup();
    ImGui::Text("Light Type");
    ImGui::RadioButton("Directional", reinterpret_cast<i32*>(&entityData.lightType), static_cast<i32>(LightType::Directional));
    ImGui::SameLine();
    ImGui::RadioButton("Point", reinterpret_cast<i32*>(&entityData.lightType), static_cast<i32>(LightType::Point));
    ImGui::SameLine();
    ImGui::RadioButton("Spot", reinterpret_cast<i32*>(&entityData.lightType), static_cast<i32>(LightType::Spot));
    ImGui::EndGroup();
  }
  else if (entityData.objectType == ObjectType::StaticMesh)
  {
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::BeginGroup();
    ImGui::Text("Mesh File");
    ImGui::Text("%s", entityData.modelPath.empty() ? "No file selected." : entityData.modelPath.string().c_str());
    ImGui::SameLine();
    if (ImGui::Button("Choose model"))
    {
      const char* filterPatterns[] = { "*.obj", "*.glb", "*.gltf", "*.fbx" };
      constexpr auto nrFilterPatterns = sizeof(filterPatterns) / sizeof(filterPatterns[0]);
      entityData.modelPath = FileDialog::OpenFileDialog("Choose 3D model", 
                                                        Paths::GetModelsPath(), 
                                                        nrFilterPatterns, 
                                                        filterPatterns, 
                                                        "3D model file (*.obj, *.glb, *.gltf, *.fbx)", 
                                                        false);
    }
    ImGui::EndGroup();
  }

  ImGui::Spacing();
  ImGui::Separator();
  ImGui::Spacing();

  auto canCreate = entityData.tagIsValid &&
    (
    (entityData.objectType == ObjectType::LightSource && entityData.lightType != LightType::None)
    ||
    (entityData.objectType == ObjectType::StaticMesh && !entityData.modelPath.empty())
    );

  if (canCreate)
  {
    if (ImGui::Button("Create"))
    {
      auto newEntity = scene.CreateObject( ... );
      newEntity.GetComponent<Tag>()->UpdateValue(entityData.tag.data());
      switch (entityData.objectType)
      {
        case ObjectType::StaticMesh:
        {
          newEntity.AddComponent<Transform>();
          auto& instance = StaticMeshFactory::GetInstance();
          auto relative = fs::relative(entityData.modelPath, Paths::GetModelsPath());
          auto prototype = instance.GetPrototype(relative);
          if (!prototype)
            prototype = instance.CreatePrototype(entityData.modelPath);

          auto& sm = newEntity.AddComponent<StaticMesh>();
          prototype->Copy(sm);
          break;
        }
        case ObjectType::LightSource:
        {
          newEntity.AddComponent<Light>();
          if (entityData.lightType == LightType::Directional) newEntity.AddComponent<DirectionalLight>();
          else if (entityData.lightType == LightType::Point) newEntity.AddComponent<PointLight>();
          else if (entityData.lightType == LightType::Spot)  newEntity.AddComponent<SpotLight>();
          break;
        }
        default:
          break;
      }
      ImGui::CloseCurrentPopup();
    }
  }
  else
  {
    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.5f);
    ImGui::Button("Create");
    ImGui::PopStyleVar();
  }
  
  ImGui::SameLine();
  
  if (ImGui::Button("Cancel"))
    ImGui::CloseCurrentPopup();
#endif
}


// ----------------------------------------------------
//          PUBLIC
// ----------------------------------------------------

void GUI_Hierarchy(bool& open, Scene& scene, GameObject& objSelected)
{
  ImGui::Begin("Hierarchy", &open);

  // "+New object" button 
  auto btnWidth = ImGui::GetContentRegionAvail().x - 32.f;
  if (Hierarchy_ButtonCentered("+New object", ImVec2(btnWidth, 26.f)))
  {
    entityData = NewEntityData{};
    ImGui::OpenPopup("New object modal");
  }
  if (ImGui::BeginPopupModal("New object modal", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
  {
    //Hierarchy_NewObjectModal(scene);
    //ImGui::EndPopup();
  }

  ImGui::Spacing();
  ImGui::Separator();
  ImGui::Spacing();

  Hierarchy_ListObjects(scene, objSelected); // List all entities

  ImGui::End();
}

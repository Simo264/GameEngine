#include "HierarchyPanel.hpp"

#include "Core/Logger.hpp"
#include "Core/FileDialog.hpp"
#include "Engine/Utils.hpp"
#include "Engine/Importers/StaticMeshLoader.hpp"
#include "Engine/Managers/TexturesManager.hpp"
#include "Engine/Managers/AssetsManager.hpp"

#include <imgui.h>

using namespace Components;

void HierarchyPanel::Render(Scene& scene)
{
  if (!isOpen)
    return;

  ImGui::Begin("Hierarchy", &isOpen);

  auto btnWidth = ImGui::GetContentRegionAvail().x - 32.f;
  if (__ButtonCentered("+New entity", Vec2I(btnWidth, 26.f)))
  {
    _entityData = EntityCreationData{};
    ImGui::OpenPopup("New entity modal");
  }
  if (ImGui::BeginPopupModal("New entity modal", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
  {
    __RenderNewEntityModal(scene);
    ImGui::EndPopup();
  }

  ImGui::Spacing();
  ImGui::Separator();
  ImGui::Spacing();

  __RenderEntityList(scene);

  ImGui::End();
}

bool HierarchyPanel::__ButtonCentered(StringView label, Vec2I size)
{
  auto avail = ImGui::GetContentRegionAvail().x;
  auto off = (avail - size.x) * 0.5f;
  if (off > 0.0f)
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);

  return ImGui::Button(label.data(), ImVec2(size.x, size.y));
}
void HierarchyPanel::__RenderNewEntityModal(Scene& scene)
{
  // Edit Tag
  ImGui::BeginGroup();
  ImGui::Text("Edit tag");
  ImGui::InputText("##Tag", _entityData.tag.data(), _entityData.tag.size());
  _entityData.tagIsValid = strlen(_entityData.tag.data()) > 0;
  if (!_entityData.tagIsValid)
  {
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Cannot be empty!");
  }
  ImGui::EndGroup();

  ImGui::Spacing();
  ImGui::Separator();
  ImGui::Spacing();

  // Archetype selection
  ImGui::BeginGroup();
  ImGui::Text("Archetype Selection");
  // Show all available arhcetypes
  auto& archetypeRegistry = scene.GetArchetypeRegistry();
  auto& availableArchetypes = archetypeRegistry.GetArchetypeVector();
  auto previousSelection = _entityData.selectedArchetypeId;
  for (auto& archetype : availableArchetypes)
  {
    auto archetypeId = archetypeRegistry.GetArchetypeId(archetype.GetName());
    auto isSelected = (_entityData.selectedArchetypeId == archetypeId);
    if (ImGui::RadioButton(archetype.GetName(), isSelected))
    {
      _entityData.selectedArchetype = &archetype;
      _entityData.selectedArchetypeId = archetypeId;
    }

    ImGui::SameLine();
  }
  ImGui::EndGroup();

  if (_entityData.selectedArchetypeId != INVALID_ARCHETYPE_ID)
  {
    auto archetypeName = StringView(_entityData.selectedArchetype->GetName());
    if (archetypeName.compare("LightSource") == 0)
    {
      ImGui::Spacing();
      ImGui::Separator();
      ImGui::Spacing();
      ImGui::BeginGroup();
      ImGui::Text("Light Type");
      ImGui::RadioButton("Directional", reinterpret_cast<i32*>(&_entityData.lightType), static_cast<i32>(LightType::Directional));
      ImGui::SameLine();
      ImGui::RadioButton("Point", reinterpret_cast<i32*>(&_entityData.lightType), static_cast<i32>(LightType::Point));
      ImGui::SameLine();
      ImGui::RadioButton("Spot", reinterpret_cast<i32*>(&_entityData.lightType), static_cast<i32>(LightType::Spot));
      ImGui::EndGroup();
    }
    else if (archetypeName.compare("StaticMesh") == 0)
    {
      ImGui::Spacing();
      ImGui::Separator();
      ImGui::Spacing();
      ImGui::BeginGroup();
      ImGui::Text("Mesh File");
      ImGui::Text("%s", _entityData.modelPath.empty() ? "No file selected." : _entityData.modelPath.string().c_str());
      ImGui::SameLine();
      if (ImGui::Button("Choose model"))
      {
        const char* filterPatterns[] = { "*.obj", "*.glb", "*.gltf", "*.fbx" };
        constexpr auto nrFilterPatterns = sizeof(filterPatterns) / sizeof(filterPatterns[0]);
        _entityData.modelPath = FileDialog::OpenFileDialog("Choose 3D model",
                                                           Utils::GetModelsPath(),
                                                           nrFilterPatterns,
                                                           filterPatterns,
                                                           "3D model file (*.obj, *.glb, *.gltf, *.fbx)",
                                                           false);
      }
      ImGui::EndGroup();
    }
    else if (archetypeName.compare("Camera") == 0)
    {
      ImGui::Text("TODO CAMERA SECTION");
    }
  }

  ImGui::Spacing();
  ImGui::Separator();
  ImGui::Spacing();

  auto canCreate = _entityData.tagIsValid &&
    _entityData.selectedArchetype != nullptr &&
    _entityData.selectedArchetypeId != INVALID_ARCHETYPE_ID &&
    (
    (StringView(_entityData.selectedArchetype->GetName()).compare("LightSource") == 0 && _entityData.lightType != LightType::None)
    ||
    (StringView(_entityData.selectedArchetype->GetName()).compare("StaticMesh") == 0) && !_entityData.modelPath.empty()
    );

  if (canCreate)
  {
    if (ImGui::Button("Create"))
    {
      auto archetypeName = StringView(_entityData.selectedArchetype->GetName());
      auto newEntity = scene.CreateEntity(_entityData.selectedArchetypeId);
      newEntity.GetComponent<Tag>()->UpdateValue(_entityData.tag.data());
      if (archetypeName.compare("StaticMesh") == 0)
      {
        newEntity.AddComponent<Transform>();

        auto& instance = AssetsManager::GetInstance();
        auto assetId = instance.GetAssetId(_entityData.modelPath);
        if (assetId == INVALID_ASSET_ID)
          assetId = instance.RegisterAsset(_entityData.modelPath);
        newEntity.AddComponent<AssetIdentifier>(assetId);

        auto& material = newEntity.AddComponent<Material>();

        auto& mesh = newEntity.AddComponent<StaticMesh>();
        mesh.Create();

        using Vertex = VertexLayout<Position, Normal, TextureCoord, Tangent>;
        Vertex::SetupVertexArray(mesh.vertexArray);

        auto loader = StaticMeshLoader{};
        loader.LoadDataFromFile(_entityData.modelPath, mesh, material);
      }
      else if (archetypeName.compare("LightSource") == 0)
      {
        newEntity.AddComponent<Light>();
        switch (_entityData.lightType)
        {
          case LightType::Directional:
            newEntity.AddComponent<DirectionalLight>();
            break;
          case LightType::Point:
            newEntity.AddComponent<PointLight>();
            break;
          case LightType::Spot:
            newEntity.AddComponent<SpotLight>();
            break;

          default:
            break;
        }
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
}
void HierarchyPanel::__RenderEntityList(Scene& scene)
{
  auto& texManager = TexturesManager::GetInstance();
  static auto icon = texManager.GetOrCreateIcon("game-object-16.png");

  auto selectableName = Array<char, 64>{};

  auto& entRegistry = scene.GetEntityRegistry();
  auto& archRegistry = scene.GetArchetypeRegistry();
  for (auto entity : entRegistry.view<EntityId>())
  {
    auto& archetypeComp = entRegistry.get<ArchetypeIdentifier>(entity);
    auto& tagComp = entRegistry.get<Tag>(entity);

    auto archetype = archRegistry.GetArchetype(archetypeComp.archetypeId);
    auto o = Entity{ entity, &scene.GetEntityRegistry(), archetype };

    selectableName.fill(0);
    std::format_to_n(selectableName.data(),
                     selectableName.size(),
                     "{}##{}",
                     tagComp.value.data(),
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

    auto isSelected = ImGui::Selectable(selectableName.data(), selectedEntity.Compare(o));
    if (isSelected && !selectedEntity.Compare(o))
      selectedEntity = o;

    if (ImGui::IsItemHovered() &&
        ImGui::IsMouseClicked(ImGuiMouseButton_Right) &&
        selectedEntity.IsValid())
      ImGui::OpenPopup("Entity Popup");

    ImGui::PopStyleColor(3);
    ImGui::EndGroup();

    ImGui::Spacing();
  }

  if (ImGui::BeginPopup("Entity Popup")) // Display menu on right click entity
  {
    if (ImGui::MenuItem("Delete entity"))
    {
      scene.DestroyEntity(selectedEntity.GetID());
      selectedEntity = Entity{ INVALID_ENTITY_ID, nullptr, nullptr };
    }
    ImGui::EndPopup();
  }
}

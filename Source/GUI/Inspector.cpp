#include "Core/Core.hpp"

#include "Core/Paths/Paths.hpp"
#include "Core/Dialog/FileDialog.hpp"

#include "Engine/Scene.hpp"
#include "Engine/Components/Components.hpp"

#include "Engine/Managers/TexturesManager.hpp"
#include "Engine/Managers/StaticMeshFactory.hpp"

#include <imgui.h>
#include <ImGuizmo.h>

using namespace Components;

static bool Insp_ButtonCentered(StringView label, ImVec2 size)
{
  auto avail = ImGui::GetContentRegionAvail().x;
  auto off = (avail - size.x) * 0.5f;
  if (off > 0.0f)
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);

  return ImGui::Button(label.data(), size);
}
static void Insp_Tag(Tag& tag)
{
  // Create a table with two columns: one for the labels and one for input
  if (ImGui::BeginTable("Tag_Table", 2, ImGuiTableFlags_SizingFixedFit))
  {
    ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthFixed, 25.f);
    ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthStretch);

    ImGui::TableNextRow();

    // Column 1: label
    ImGui::TableNextColumn();
    ImGui::Text("Tag");

    // Column 2: input
    ImGui::TableNextColumn();
    auto buffer = Array<char, 64>{};
    std::format_to_n(buffer.data(), buffer.size(), "{}", tag.value.data());

    if (ImGui::InputText("##Value", buffer.data(), buffer.size(), ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_EnterReturnsTrue))
    {
      auto value = StringView(buffer.data());
      if (value.size() > 0)
        tag.UpdateValue(value);
    }

    ImGui::EndTable();
  }
}
static void Insp_Light_ComboAttenuation(f32& kl, f32& kq)
{
  static auto buffer = Array<char, 64>{}; // Buffer statico riutilizzabile
  buffer.fill(0);

  // Find current selection using std::find_if with lambda
  auto currentIt = std::find_if(ATTENUATION_RANGES.begin(), 
                                ATTENUATION_RANGES.end(), 
                                [kl, kq](const Array<f32, 3>& data) 
  {
    return std::abs(data[1] - kl) < 0.001f && std::abs(data[2] - kq) < 0.001f;
  });

  auto currentSelection = (currentIt != ATTENUATION_RANGES.end()) ?
    std::distance(ATTENUATION_RANGES.begin(), currentIt) : -1;

  // Create concise preview string
  if (currentSelection >= 0)
  {
    std::snprintf(buffer.data(), buffer.size(), "Range %d", static_cast<i32>((*currentIt)[0]));
  }
  else
  {
    constexpr auto text = StringView("Custom");
    constexpr auto len = text.length();
    std::copy_n(text.begin(), len, buffer.begin());
  }


  if (ImGui::BeginCombo("##AttenuationCombo", buffer.data()))
  {
    // Add "Custom" option at the top
    auto isCustomSelected = (currentSelection == -1);
    if (ImGui::Selectable("Custom", isCustomSelected))
    {
      // Don't change values, just close combo
    }

    // Add separator
    ImGui::Separator();

    // Add preset options using enumerate pattern
    auto index = 0;
    std::for_each(ATTENUATION_RANGES.begin(), 
                  ATTENUATION_RANGES.end(),
                  [&](const Array<f32, 3>& data) 
    {
      std::snprintf(buffer.data(), 
                    buffer.size(), 
                    "Range %d (kl: %.3f, kq: %.4f)", 
                    static_cast<int>(data[0]), data[1], data[2]);

      auto isSelected = (currentSelection == index);
      if (ImGui::Selectable(buffer.data(), isSelected))
      {
        kl = data[1];
        kq = data[2];
      }
      if (isSelected)
        ImGui::SetItemDefaultFocus();
      
      ++index;
    });

    ImGui::EndCombo();
  }

  // Show current values as text below the combo
  ImGui::SameLine();
  if (ImGui::Button("Edit"))
  {
    ImGui::OpenPopup("EditAttenuation");
  }
  if (ImGui::BeginPopup("EditAttenuation"))
  {
    ImGui::Text("Manual Attenuation Values");
    ImGui::Separator();

    ImGui::DragFloat("Linear (kl)", &kl, 0.001f, 0.0f, 2.0f, "%.4f");
    ImGui::DragFloat("Quadratic (kq)", &kq, 0.001f, 0.0f, 2.0f, "%.6f");

    if (ImGui::Button("Close"))
    {
      ImGui::CloseCurrentPopup();
    }

    ImGui::EndPopup();
  }
}
static void Insp_DirectLight(GameObject object, DirectionalLight& light)
{
  // Create a table with two columns: one for the labels and one for input
  if (ImGui::BeginTable("Light_Table", 2, ImGuiTableFlags_SizingFixedFit))
  {
    ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthFixed, 80.f);
    ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthStretch);

    // Row 1: view light type
    ImGui::TableNextRow();
    ImGui::TableNextColumn(); // Column 1: label
    ImGui::Text("Type");
    ImGui::TableNextColumn(); // Second column: combo
    ImGui::BeginDisabled();
    if (ImGui::BeginCombo("##Type", "Directional"))
      ImGui::EndCombo();
    ImGui::EndDisabled();

    // Row 2: color input
    ImGui::TableNextRow();
    ImGui::TableNextColumn(); // Column 1: label
    ImGui::Text("Color");
    ImGui::TableNextColumn(); // Column 2: input
    ImGui::ColorEdit3("##Color", reinterpret_cast<f32*>(&light.color));

    // Row 3: intensity input
    ImGui::TableNextRow();
    ImGui::TableNextColumn(); // Column 1: label
    ImGui::Text("Intensity");
    ImGui::TableNextColumn(); // Column 2: input
    ImGui::SliderFloat("##Intensity", &light.intensity, 0.0f, 1.0f);

    // Row 4: direction input
    ImGui::TableNextRow();
    ImGui::TableNextColumn(); // Column 1: label
    ImGui::Text("Direction");
    ImGui::TableNextColumn(); // Column 2: input
    ImGui::DragFloat3("##Direction", reinterpret_cast<f32*>(&light.direction), 0.1f, -FLT_MAX, FLT_MAX);

    ImGui::EndTable();
  }

  ImGui::SeparatorText("Advanced");
  ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.55f, 0.f, 0.f, 0.5f));
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.45f, 0.f, 0.f, 0.5f));
  ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.45f, 0.f, 0.f, 0.5f));
  if (ImGui::Button("Remove component##dir_light"))
  {
    object.RemoveComponent<Light>();
    object.RemoveComponent<DirectionalLight>();
  }
  ImGui::PopStyleColor(3);
}
static void Insp_PointLight(GameObject object, PointLight& light)
{
  // Create a table with two columns: one for the labels and one for input
  if (ImGui::BeginTable("Light_Table", 2, ImGuiTableFlags_SizingFixedFit))
  {
    ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthFixed, 80.f);
    ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthStretch);

    // Row 1: view light type
    ImGui::TableNextRow();
    ImGui::TableNextColumn(); // Column 1: label
    ImGui::Text("Type");
    ImGui::TableNextColumn(); // Column 2: combo
    ImGui::BeginDisabled();
    if (ImGui::BeginCombo("##Type", "Point"))
      ImGui::EndCombo();
    ImGui::EndDisabled();

    // Row 2: color input
    ImGui::TableNextRow();
    ImGui::TableNextColumn(); // Column 1: label
    ImGui::Text("Color");
    ImGui::TableNextColumn(); // Column 2: input
    ImGui::ColorEdit3("##Color", reinterpret_cast<f32 *>(&light.color));

    // Row 3: diffuse input
    ImGui::TableNextRow();
    ImGui::TableNextColumn(); // Column 1: label
    ImGui::Text("Intensity");
    ImGui::TableNextColumn(); // Column 2: input
    ImGui::SliderFloat("##Intensity", &light.intensity, 0.0f, 1.0f);

    // Row 4: position input
    ImGui::TableNextRow();
    ImGui::TableNextColumn(); // Column 1: label
    ImGui::Text("Position");
    ImGui::TableNextColumn(); // Column 2: input
    ImGui::DragFloat3("##Position", reinterpret_cast<f32 *>(&light.position), 0.1f, -FLT_MAX, FLT_MAX);

    // Row 4: attenuation combo
    ImGui::TableNextRow();
    ImGui::TableNextColumn(); // Column 1: label
    ImGui::TextWrapped("Attenuation");
    ImGui::TableNextColumn(); // Column 2: input
    Insp_Light_ComboAttenuation(light.kl, light.kq);
    ImGui::EndTable();
  }

  ImGui::SeparatorText("Advanced");
  ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.55f, 0.f, 0.f, 0.5f));
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.45f, 0.f, 0.f, 0.5f));
  ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.45f, 0.f, 0.f, 0.5f));
  if (ImGui::Button("Remove component##point_light"))
  {
    object.RemoveComponent<Light>();
    object.RemoveComponent<PointLight>();
  }
  ImGui::PopStyleColor(3);
}
static void Insp_SpoLight(GameObject object, SpotLight& light)
{
  // Crea una tabella con due colonne per le etichette e gli input
  if (ImGui::BeginTable("Spotlight_Table", 2, ImGuiTableFlags_SizingFixedFit))
  {
    ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthFixed, 80.f);
    ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthStretch);

    // Riga 1: tipo di luce (fisso su "Spot")
    ImGui::TableNextRow();
    ImGui::TableNextColumn(); // Colonna 1: etichetta
    ImGui::Text("Type");
    ImGui::TableNextColumn(); // Colonna 2: combo disabilitato
    ImGui::BeginDisabled();
    if (ImGui::BeginCombo("##Type", "Spot"))
      ImGui::EndCombo();
    ImGui::EndDisabled();

    // Riga 2: input colore
    ImGui::TableNextRow();
    ImGui::TableNextColumn(); // Colonna 1: etichetta
    ImGui::Text("Color");
    ImGui::TableNextColumn(); // Colonna 2: input
    ImGui::ColorEdit3("##Color", reinterpret_cast<f32*>(&light.color));

    // Riga 3: input intensità
    ImGui::TableNextRow();
    ImGui::TableNextColumn(); // Colonna 1: etichetta
    ImGui::Text("Intensity");
    ImGui::TableNextColumn(); // Colonna 2: input
    ImGui::SliderFloat("##Intensity", &light.intensity, 0.0f, 1.0f);

    // Riga 4: input posizione
    ImGui::TableNextRow();
    ImGui::TableNextColumn(); // Colonna 1: etichetta
    ImGui::Text("Position");
    ImGui::TableNextColumn(); // Colonna 2: input
    ImGui::DragFloat3("##Position", reinterpret_cast<f32*>(&light.position), 0.1f, -FLT_MAX, FLT_MAX);

    // Riga 5: input direzione
    ImGui::TableNextRow();
    ImGui::TableNextColumn(); // Colonna 1: etichetta
    ImGui::Text("Direction");
    ImGui::TableNextColumn(); // Colonna 2: input
    ImGui::DragFloat3("##Direction", reinterpret_cast<f32*>(&light.direction), 0.01f, -1.0f, 1.0f);

    // Riga 6: attenuazione (con un combo personalizzato o slider)
    ImGui::TableNextRow();
    ImGui::TableNextColumn(); // Colonna 1: etichetta
    ImGui::TextWrapped("Attenuation");
    ImGui::TableNextColumn(); // Colonna 2: input
    // Insp_Light_ComboAttenuation(light.kl, light.kq); // Se vuoi usare un combo
    ImGui::DragFloat("##Linear", &light.kl, 0.001f, 0.0f, 1.0f, "Linear");
    ImGui::DragFloat("##Quadratic", &light.kq, 0.001f, 0.0f, 1.0f, "Quadratic");

    // Riga 7: angoli del cono
    ImGui::TableNextRow();
    ImGui::TableNextColumn(); // Colonna 1: etichetta
    ImGui::TextWrapped("Cone Angles");
    ImGui::TableNextColumn(); // Colonna 2: input
    ImGui::SliderFloat("##Outer Angle", &light.thetaU, 0.0f, 90.0f, "Outer Angle (%.1f deg)");
    ImGui::SliderFloat("##Inner Angle", &light.thetaP, 0.0f, 90.0f, "Inner Angle (%.1f deg)");

    ImGui::EndTable();
  }

  // Sezione per rimuovere il componente
  ImGui::SeparatorText("Advanced");
  ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.55f, 0.f, 0.f, 0.5f));
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.45f, 0.f, 0.f, 0.5f));
  ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.45f, 0.f, 0.f, 0.5f));
  if (ImGui::Button("Remove component##spot_light"))
  {
    object.RemoveComponent<Light>();
    object.RemoveComponent<SpotLight>();
  }
  ImGui::PopStyleColor(3);
}

static void Insp_Transformation_TableRow(StringView label, Vec3F& values, f32 resetValue)
{
  ImGui::PushID(label.data()); // label = "Position" or "Rotation" or "Scale"

  // Column 1: label
  ImGui::TableNextColumn();
  ImGui::Text(label.data());

  // Column 2: inputs
  ImGui::TableNextColumn();
  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{0, 0});

  auto lineHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
  auto buttonSize = ImVec2{ lineHeight + 3.0f, lineHeight };
  auto itemWidth = ImGui::CalcItemWidth() / 3.0f;

  // X value
  ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.8f, 0.1f, 0.15f, 1.0f});
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.9f, 0.2f, 0.2f, 1.0f});
  ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.7f, 0.05f, 0.1f, 1.0f});
  if (ImGui::Button("X", buttonSize))
    values.x = resetValue;
  ImGui::PopStyleColor(3);
  ImGui::SameLine();
  ImGui::SetNextItemWidth(itemWidth);
  ImGui::DragFloat("##X", &values.x, 0.1f);
  ImGui::SameLine();

  // Y value
  ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.2f, 0.7f, 0.2f, 1.0f});
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.3f, 0.8f, 0.3f, 1.0f});
  ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.1f, 0.6f, 0.1f, 1.0f});
  if (ImGui::Button("Y", buttonSize))
    values.y = resetValue;
  ImGui::PopStyleColor(3);
  ImGui::SameLine();
  ImGui::SetNextItemWidth(itemWidth);
  ImGui::DragFloat("##Y", &values.y, 0.1f);
  ImGui::SameLine();

  // Z value
  ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.1f, 0.25f, 0.8f, 1.0f});
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.2f, 0.35f, 0.9f, 1.0f});
  ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.05f, 0.2f, 0.7f, 1.0f});
  if (ImGui::Button("Z", buttonSize))
    values.z = resetValue;
  ImGui::PopStyleColor(3);
  ImGui::SameLine();
  ImGui::SetNextItemWidth(itemWidth);
  ImGui::DragFloat("##Z", &values.z, 0.1f);
  
  ImGui::PopStyleVar();
  ImGui::PopID();
}
static void Insp_Transformation(GameObject object, Transform& transformation)
{
  // Create a table with two columns: one for the labels and one for inputs
  if (ImGui::BeginTable("Transformation_Table", 2, ImGuiTableFlags_SizingFixedFit))
  {
    ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthFixed, 80.f);
    ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthStretch);

    // Row 1: position
    ImGui::TableNextRow();
    Insp_Transformation_TableRow("Position", transformation.position, 0.f);

    // Row 2: Rotation
    ImGui::TableNextRow();
    Insp_Transformation_TableRow("Rotation", transformation.eulerAngles, 0.f);

    // Row 3: Scale
    ImGui::TableNextRow();
    Insp_Transformation_TableRow("Scale", transformation.scale, 1.f);

    ImGui::EndTable();
  }

  ImGui::SeparatorText("Advanced");
  ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.55f, 0.f, 0.f, 0.5f));
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.45f, 0.f, 0.f, 0.5f));
  ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.45f, 0.f, 0.f, 0.5f));
  if (ImGui::Button("Remove component##transform"))
    object.RemoveComponent<Transform>();
  ImGui::PopStyleColor(3);
}
static void Insp_ShowTextureSelector(StringView label, 
                                     u32 meshID, 
                                     Texture2D& meshTexture, 
                                     Texture2D defaultTex)
{
  auto& texManager = TexturesManager::GetInstance();
  auto isMeshTextureValid = meshTexture.GetWidth() != 1;

  // Column 1: label
  ImGui::TableNextColumn();
  ImGui::Text(label.data());

  // Column 2: combo
  ImGui::TableNextColumn();
  ImGui::SetNextItemWidth(ImGui::GetColumnWidth());
  if (isMeshTextureValid)
  {
    auto texturePath = texManager.GetTexturePath(meshTexture.id);
    ImGui::TextWrapped(texturePath->string().c_str());
  }
  else
    ImGui::TextWrapped("No texture");

  // Column 3: reset button
  ImGui::TableNextColumn();
  if (isMeshTextureValid)
  {
    static auto resetIcon = texManager.GetOrCreateIcon("reset-arrow-16.png");

    auto buttonID = Array<char, 64>{}; // e.g. "Reset##Mesh_1_Diffuse"
    std::format_to(buttonID.begin(), "Reset##Mesh_{}_{}", meshID, label.data());

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.f, 0.f, 0.f, 0.f});
    if (ImGui::ImageButton(buttonID.data(), resetIcon.id, ImVec2(16.f, 16.f)))
      meshTexture = defaultTex;
    ImGui::PopStyleColor();
  }


  // "Choose texture" button
  ImGui::TableNextRow();
  ImGui::TableSetColumnIndex(1); // Column 2
  {
    auto buttonID = Array<char, 64>{}; // e.g. "Reset##Mesh_1_Diffuse"
    std::format_to(buttonID.begin(), "Choose texture##Mesh_{}_{}", meshID, label.data());
    if (ImGui::Button(buttonID.data()))
    {
      const char* filterPatterns[] = { "*.png", "*.jpg" };
      constexpr auto nrFilterPatterns = sizeof(filterPatterns) / sizeof(filterPatterns[0]);
      auto path = FileDialog::OpenFileDialog("Choose a texture for the 3D mesh",
                                             Paths::GetTexturesPath(),
                                             nrFilterPatterns,
                                             filterPatterns,
                                             "Image files (*.png, *jpg)",
                                             false);
      if (!path.empty())
      {
        path = fs::relative(path, Paths::GetTexturesPath());
        meshTexture = texManager.GetOrCreateTexture(path);
      }
    }
  }
}
static void Insp_StaticMesh(GameObject object, StaticMesh& staticMesh)
{
  auto& texManager = TexturesManager::GetInstance();
  
  ImGui::Text("Nr meshes: %d", staticMesh.nrMeshes);
  // View meshes with a tree
  if (ImGui::TreeNode("Material"))
  {
    for (auto i = 0u; i < staticMesh.nrMeshes; i++)
    {
      auto& mesh = staticMesh.meshArray[i];
      auto& material = mesh.material;

      auto meshLabel = Array<char, 32>{};
      std::format_to(meshLabel.begin(), "Mesh_{}", i + 1);
      if (ImGui::TreeNode(meshLabel.data()))
      {
        if (ImGui::BeginTable("TextureTable", 3, ImGuiTableFlags_SizingFixedFit))
        {
          ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthFixed, 48.0f);
          ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthStretch);
          ImGui::TableSetupColumn(nullptr, ImGuiTableColumnFlags_WidthFixed, 24.0f);

          // Diffuse row
          ImGui::TableNextRow();
          Insp_ShowTextureSelector("Diffuse", i, material.albedo, texManager.GetDefaultDiffuse());

          // Specular row
          //ImGui::TableNextRow();
          //Insp_ShowTextureSelector("Specular", i, material.specular, texManager.GetDefaultSpecular());

          // Normal row
          ImGui::TableNextRow();
          Insp_ShowTextureSelector("Normal", i, material.normalMap, texManager.GetDefaultNormal());

          ImGui::EndTable();
        }
        ImGui::TreePop();
      }
    }

    ImGui::TreePop();
  }

  ImGui::SeparatorText("Advanced");
  ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.55f, 0.f, 0.f, 0.5f));
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.45f, 0.f, 0.f, 0.5f));
  ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.45f, 0.f, 0.f, 0.5f));
  if (ImGui::Button("Remove component##model"))
    object.RemoveComponent<StaticMesh>();
  ImGui::PopStyleColor(3);
}


static void Insp_AddTransformationComponent(GameObject object)
{
  if (object.HasComponent<Transform>())
  {
    ImGui::Selectable("Transform", false, ImGuiSelectableFlags_Disabled);
  }
  else if (ImGui::Selectable("Transform"))
  {
    object.AddComponent<Transform>();
  }
}
static void Insp_AddLightComponent(GameObject object)
{
  if (object.HasComponent<Light>())
  {
    ImGui::Selectable("Light", false, ImGuiSelectableFlags_Disabled);
  }
  else if (ImGui::BeginMenu("Light"))
  {
    if (ImGui::MenuItem("Directional"))
    {
      object.AddComponent<Light>(LightType::Directional);
      object.AddComponent<DirectionalLight>();
    }
    if (ImGui::MenuItem("Point"))
    {
      object.AddComponent<Light>(LightType::Point);
      object.AddComponent<PointLight>();
    }
    if (ImGui::MenuItem("Spot"))
    {
      object.AddComponent<Light>(LightType::Spot);
      object.AddComponent<SpotLight>();
    }
    ImGui::EndMenu();
  }
}
static void Insp_AddStaticMeshComponent(GameObject object)
{
  if (object.HasComponent<StaticMesh>())
  {
    ImGui::Selectable("StaticMesh", false, ImGuiSelectableFlags_Disabled);
  }
  else if (ImGui::BeginMenu("StaticMesh"))
  {
    ImGui::BeginChild("StaticMesh_Child", ImVec2(300, 100));
    if (ImGui::Button("Choose static mesh"))
    {
      const char* filterPatterns[] = { "*.obj", "*.glb", "*.gltf", "*.fbx" };
      constexpr auto nrFilterPatterns = sizeof(filterPatterns) / sizeof(filterPatterns[0]);
      auto path = FileDialog::OpenFileDialog("Choose 3D model",
                                             Paths::GetModelsPath(),
                                             nrFilterPatterns,
                                             filterPatterns,
                                             "3D model file (*.obj, *.glb, *.gltf, *.fbx)",
                                             false);


      if (!path.empty())
      {
        auto& instance = StaticMeshFactory::GetInstance();

        auto relative = fs::relative(path, Paths::GetModelsPath());
        auto prototype = instance.GetPrototype(relative);
        if (!prototype)
          prototype = instance.CreatePrototype(path);
        
        auto& sm = object.AddComponent<StaticMesh>();
        prototype->Copy(sm);
      }
    }
    ImGui::EndChild();
    ImGui::EndMenu();
  }
}

static void Insp_ListAllComponents(GameObject object)
{
  if (ImGui::CollapsingHeader("Tag"))
  {
    auto tag = object.GetComponent<Tag>();
    Insp_Tag(*tag);
  }
  if (auto transform = object.GetComponent<Transform>())
  {
    if (ImGui::CollapsingHeader("Transform"))
      Insp_Transformation(object, *transform);
  }
  if (auto staticMesh = object.GetComponent<StaticMesh>())
  {
    if (ImGui::CollapsingHeader("StaticMesh"))
      Insp_StaticMesh(object, *staticMesh);
  }
  if (auto light = object.GetComponent<Light>())
  {
    if (ImGui::CollapsingHeader("Light"))
    {
      switch (light->type)
      {
        case LightType::Directional:
        {
          auto dirLight = object.GetComponent<DirectionalLight>();
          Insp_DirectLight(object, *dirLight);
          break;
        }
        case LightType::Point:
        {
          auto pointLight = object.GetComponent<PointLight>();
          Insp_PointLight(object, *pointLight);
          break;
        }
        case LightType::Spot:
        {
          auto spotLight = object.GetComponent<SpotLight>();
          Insp_SpoLight(object, *spotLight);
          break;
        }
      }
    }
  }
}
static void Insp_NewComponentPopup(GameObject object)
{
  if (ImGui::BeginPopup("NewComponent_Popup"))
  {
    // Add Transform component
    // --------------------------------
    Insp_AddTransformationComponent(object);

    ImGui::Spacing();

    // Add Light component
    // --------------------------------
    Insp_AddLightComponent(object);

    ImGui::Spacing();

    // Add StaticMesh component
    // --------------------------------
    Insp_AddStaticMeshComponent(object);

    ImGui::EndPopup();
  }
}

// ------------------------------------------
//                    PUBLIC
// ------------------------------------------

void GUI_Inspector(bool& open, StringView windowName, GameObject object)
{
  ImGui::Begin(windowName.data(), &open);
  if (object.IsValid())
  {
    // "+New component" button
    auto btnWidth = ImGui::GetContentRegionAvail().x - 32.f;
    if (Insp_ButtonCentered("+Add component", ImVec2(btnWidth, 26.f)))
      ImGui::OpenPopup("NewComponent_Popup");

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // List all components
    Insp_ListAllComponents(object);
  }

  // Open popup on click to "+New component" button
  Insp_NewComponentPopup(object);

  ImGui::End();
}

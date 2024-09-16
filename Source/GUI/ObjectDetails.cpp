#include "Core/Core.hpp"

#include "Engine/ECS/ECS.hpp"
#include "Engine/Subsystems/TextureManager.hpp"

#include <imgui/imgui.h>
#include <imgui/ImGuizmo.h>

/* ------------------------------------------ */
/*                  PRIVATE                   */
/* ------------------------------------------ */


static constexpr const char* ATTENUATION_LABELS[]{
  "7m",
  "13m",
  "20m",
  "32m",
  "50m",
  "65m",
  "100m",
  "160m",
  "200m",
  "325m",
  "600m",
  "3250m"
};
static constexpr Array<Tuple<f32, f32>, 12> ATTENUATION_VALUES = {
  Tuple(0.7f, 1.8f),         /* 7 meters */
  Tuple(0.35f, 0.44f),       /* 13 meters */
  Tuple(0.22f, 0.20f),       /* 20 meters */
  Tuple(0.14f, 0.07f),       /* 32 meters */
  Tuple(0.09f, 0.032f),      /* 50 meters */
  Tuple(0.07f, 0.017f),      /* 65 meters */
  Tuple(0.045f, 0.0075f),    /* 100 meters */
  Tuple(0.027f, 0.0028f),    /* 160 meters */
  Tuple(0.022f, 0.0019f),    /* 200 meters */
  Tuple(0.014f, 0.0007f),    /* 325 meters */
  Tuple(0.007f, 0.0002f),    /* 600 meters */
  Tuple(0.0014f, 0.000007f), /* 3250 meters */
};

static void ComboTextures(Texture2D*& matTexture, StringView comboId, const Texture2D& defaultTex)
{
  const bool isEmpty = (matTexture->strPath.at(0) == '#');
  auto& textManager = TextureManager::Get();

  f32 panelWidth = ImGui::GetContentRegionAvail().x;
  ImGui::SetNextItemWidth(panelWidth - 32);
  if (ImGui::BeginCombo(comboId.data(), (isEmpty ? "Select texture" : matTexture->strPath.c_str())))
  {
    for (const auto& [key, texture] : textManager.GetTextures())
    {
      if (key.at(0) != '#' && ImGui::Selectable(key.c_str(), texture.Compare(*matTexture)))
        matTexture = const_cast<Texture2D*>(&texture);
    }
    ImGui::EndCombo();
  }

  if (!isEmpty)
  {
    ImGui::SameLine();

    char buttonID[32]{};
    std::format_to_n(buttonID, sizeof(buttonID), "Reset{}", comboId.data());

    const auto& resetIcon = textManager.GetIconByPath(GetIconsPath() / "reset-arrow.png");
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.f,0.f,0.f,0.f });
    if (ImGui::ImageButton(buttonID, reinterpret_cast<void*>(resetIcon.id), { 16,16 }))
      matTexture = const_cast<Texture2D*>(&defaultTex);
    ImGui::PopStyleColor();
  }
}

/* ------------------------------------------ */
/*                    PUBLIC                  */
/* ------------------------------------------ */


void GUI_RenderObjectDetails(bool& open, GameObject& object, i32& gizmode)
{
  ImGui::Begin("Details", &open);
  if (object.IsValid())
  {
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.f, 0.85f, 0.f, 1.f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
    if (ImGui::Button("+Add component"))
    {
      CONSOLE_DEBUG("TODO");
    }
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();


    if (auto* light = object.GetComponent<DirectionalLight>())
    {
      if (ImGui::CollapsingHeader("Directional light", ImGuiTreeNodeFlags_DefaultOpen))
      {
        ImGui::ColorEdit3("Color", (f32*)&light->color);
        ImGui::SliderFloat("Diffuse intensity", &light->diffuseIntensity, 0.0f, 1.0f);
        ImGui::SliderFloat("Specular intensity", &light->specularIntensity, 0.0f, 1.0f);
        ImGui::DragFloat3("Direction", (f32*)&light->direction, 0.1f, -FLT_MAX, FLT_MAX);

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.55f, 0.f, 0.f, 0.5f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.45f, 0.f, 0.f, 0.5f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.45f, 0.f, 0.f, 0.5f));
        if (ImGui::Button("Remove component##dir_light"))
          object.RemoveComponent<DirectionalLight>();
        ImGui::PopStyleColor(3);
      }
    }
    if (auto* light = object.GetComponent<PointLight>())
    {
      if (ImGui::CollapsingHeader("Point light", ImGuiTreeNodeFlags_DefaultOpen))
      {
        ImGui::ColorEdit3("Color", (f32*)&light->color);
        ImGui::SliderFloat("Diffuse intensity", &light->diffuseIntensity, 0.0f, 1.0f);
        ImGui::SliderFloat("Specular intensity", &light->specularIntensity, 0.0f, 1.0f);
        ImGui::DragFloat3("Position", (f32*)&light->position, 0.1f, -FLT_MAX, FLT_MAX);

        ImGui::Text("Attenuation");
        ImGui::Separator();
        static i32 index = 0;
        if (ImGui::Combo("Distance", &index, ATTENUATION_LABELS, IM_ARRAYSIZE(ATTENUATION_LABELS)))
        {
          const auto& selected = ATTENUATION_VALUES.at(index);
          light->attenuation.kl = std::get<0>(selected);
          light->attenuation.kq = std::get<1>(selected);
        }

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.55f, 0.f, 0.f, 0.5f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.45f, 0.f, 0.f, 0.5f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.45f, 0.f, 0.f, 0.5f));
        if (ImGui::Button("Remove component##point_light"))
          object.RemoveComponent<PointLight>();
        ImGui::PopStyleColor(3);
      }
    }
    if (auto* light = object.GetComponent<SpotLight>())
    {
      if (ImGui::CollapsingHeader("Spot light", ImGuiTreeNodeFlags_DefaultOpen))
      {
        ImGui::ColorEdit3("Color", (f32*)&light->color);
        ImGui::SliderFloat("Diffuse intensity", &light->diffuseIntensity, 0.0f, 1.0f);
        ImGui::SliderFloat("Specular intensity", &light->specularIntensity, 0.0f, 1.0f);
        ImGui::DragFloat3("Direction", (f32*)&light->direction, 0.1f, -FLT_MAX, FLT_MAX);
        ImGui::DragFloat3("Position", (f32*)&light->position, 0.1f, -FLT_MAX, FLT_MAX);

        ImGui::Text("Radius");
        ImGui::Separator();
        ImGui::SliderFloat("Inner cutoff", &light->cutOff, 1.0f, light->outerCutOff);
        ImGui::SliderFloat("Outer cutoff", &light->outerCutOff, light->cutOff, 45.0f);

        ImGui::Text("Attenuation");
        ImGui::Separator();
        static i32 index = 0;
        if (ImGui::Combo("Distance", &index, ATTENUATION_LABELS, IM_ARRAYSIZE(ATTENUATION_LABELS)))
        {
          const auto& selected = ATTENUATION_VALUES.at(index);
          light->attenuation.kl = std::get<0>(selected);
          light->attenuation.kq = std::get<1>(selected);
        }

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.55f, 0.f, 0.f, 0.5f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.45f, 0.f, 0.f, 0.5f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.45f, 0.f, 0.f, 0.5f));
        if (ImGui::Button("Remove component##spot_light"))
          object.RemoveComponent<SpotLight>();
        ImGui::PopStyleColor(3);
      }
    }
    if (auto* transform = object.GetComponent<Transform>())
    {
      if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
      {
        ImGui::RadioButton("Translate", &gizmode, ImGuizmo::OPERATION::TRANSLATE);
        ImGui::RadioButton("Rotate", &gizmode, ImGuizmo::OPERATION::ROTATE);
        ImGui::RadioButton("Scale", &gizmode, ImGuizmo::OPERATION::SCALE);

        ImGui::DragFloat3("Position", (f32*)&transform->position, 0.1f, -FLT_MAX, FLT_MAX);
        ImGui::DragFloat3("Scale", (f32*)&transform->scale, 0.1f, -FLT_MAX, FLT_MAX);
        ImGui::DragFloat3("Rotation", (f32*)&transform->rotation, 0.1f, -FLT_MAX, FLT_MAX);
        transform->UpdateTransformation();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.55f, 0.f, 0.f, 0.5f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.45f, 0.f, 0.f, 0.5f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.45f, 0.f, 0.f, 0.5f));
        if (ImGui::Button("Remove component##transform"))
          object.RemoveComponent<Transform>();
        ImGui::PopStyleColor(3);
      }
    }
    if (auto* model = object.GetComponent<Model>())
    {
      if (ImGui::CollapsingHeader("Model", ImGuiTreeNodeFlags_DefaultOpen))
      {
        /* Display model path */
        ImGui::TextWrapped("Model: %s", model->strPath.c_str());
        ImGui::Separator();

        /* Display the number of meshes */
        const i64 numMeshes = model->meshes.size();
        ImGui::TextWrapped("Meshes: %d", numMeshes);

        /* For each mesh */
        for (i32 i = 0; i < numMeshes; i++)
        {
          auto& mesh = model->meshes.at(i);
          Material& material = mesh.material;
          TextureManager& texManager = TextureManager::Get();

          /* Display mesh-i props */
          char title[16]{};
          std::format_to_n(title, sizeof(title), "Mesh {}", i);
          ImGui::SeparatorText(title);

          ImGui::Text("Diffuse");
          ComboTextures(material.diffuse, "##Diffuse", texManager.GetDefaultDiffuse());
          ImGui::InvisibleButton("##margin_bottom", ImVec2(1.f, 5.0f));

          ImGui::Text("Specular");
          ComboTextures(material.specular, "##Specular", texManager.GetDefaultSpecular());
          ImGui::InvisibleButton("##margin_bottom", ImVec2(1.f, 5.0f));

          ImGui::Text("Normal");
          ComboTextures(material.normal, "##Normal", texManager.GetDefaultNormal());
          ImGui::InvisibleButton("##margin_bottom", ImVec2(1.f, 5.0f));

          ImGui::Text("Height");
          ComboTextures(material.height, "##Height", texManager.GetDefaultHeight());
        }

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.55f, 0.f, 0.f, 0.5f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.45f, 0.f, 0.f, 0.5f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.45f, 0.f, 0.f, 0.5f));
        if (ImGui::Button("Remove component##model"))
          object.RemoveComponent<Model>();
        ImGui::PopStyleColor(3);
      }
    }
  }
  ImGui::End();
}

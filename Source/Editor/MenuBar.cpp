#include "MenuBar.hh"

#include "Imgui/imgui.h"
#include "Imgui/imgui_impl_glfw.h"
#include "Imgui/imgui_impl_opengl3.h"
#include "Imgui/imgui_stdlib.h"
#include "Imgui/imgui_internal.h"

MenuBar::MenuBar(const Array<bool*, MENU_ITEMS_COUNT>& menuItems)
{
  items = menuItems;
}

void MenuBar::RenderMenuBar()
{
  if (ImGui::BeginMenuBar())
  {
    if (ImGui::BeginMenu("File"))
    {
      ImGui::MenuItem("New", nullptr, items[NEW_SCENE]);
      ImGui::MenuItem("Save", nullptr, items[SAVE_SCENE]);
      ImGui::MenuItem("Open...", nullptr, items[OPEN_SCENE]);
      ImGui::MenuItem("Save as...", nullptr, items[SAVE_AS_SCENE]);
      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("View"))
    {
      ImGui::MenuItem("Outliner panel",   nullptr, items[PANEL_OUTLINER]);
      ImGui::MenuItem("Content browser",  nullptr, items[PANEL_BROWSER]);
      ImGui::MenuItem("Inspector panel",  nullptr, items[PANEL_INSPECTOR]);
      ImGui::MenuItem("Viewport panel",   nullptr, items[PANEL_VIEWPORT]);
      ImGui::MenuItem("Details panel",    nullptr, items[PANEL_DETAILS]);
      ImGui::MenuItem("Debug frame",      nullptr, items[FRAME_DEBUG]);
      ImGui::MenuItem("Demo",             nullptr, items[FRAME_DEMO]);
      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Preferences"))
    {
      if (ImGui::MenuItem("Settings"))
      {
        *items[FRAME_SETTINGS] = true;
      }
      ImGui::EndMenu();
    }
    ImGui::EndMenuBar();
  }
}

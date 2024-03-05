#include "MenuBar.hpp"

#include <imgui/imgui.h>

MenuBar::MenuBar(const Array<bool*, MENU_ITEMS_COUNT>& menuItems)
{
  _items = menuItems;
}

void MenuBar::RenderMenuBar()
{
  if (ImGui::BeginMenuBar())
  {
    if (ImGui::BeginMenu("File"))
    {
      ImGui::MenuItem("New scene", nullptr, _items[NEW_SCENE]);
      ImGui::MenuItem("Open scene...", nullptr, _items[OPEN_SCENE]);
      //ImGui::MenuItem("Save scene", nullptr, _items[SAVE_SCENE]);
      ImGui::MenuItem("Save scene as...", nullptr, _items[SAVE_AS_SCENE]);
      ImGui::Separator();
      ImGui::MenuItem("Exit", nullptr, _items[EXIT]);
      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("View"))
    {
      ImGui::MenuItem("Outliner panel",   nullptr, _items[PANEL_OUTLINER]);
      ImGui::MenuItem("Content browser",  nullptr, _items[PANEL_BROWSER]);
      ImGui::MenuItem("Inspector panel",  nullptr, _items[PANEL_INSPECTOR]);
      ImGui::MenuItem("Viewport panel",   nullptr, _items[PANEL_VIEWPORT]);
      ImGui::MenuItem("Details panel",    nullptr, _items[PANEL_DETAILS]);
      ImGui::MenuItem("Debug frame",      nullptr, _items[FRAME_DEBUG]);
      ImGui::MenuItem("Demo",             nullptr, _items[FRAME_DEMO]);
      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Preferences"))
    {
      ImGui::MenuItem("Settings", nullptr, _items[FRAME_SETTINGS]);
      ImGui::EndMenu();
    }

    ImGui::EndMenuBar();
  }
}

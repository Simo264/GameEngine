#pragma once

#include "Core/Core.hpp"

namespace ImGuiLayer
{
	void SetupContext();

	void CleanUp();

	void SetFont(const fspath& fontpath, int fontsize);

	void NewFrame();

	void DrawData();

	void RenderMenuBar();

	void RenderOutlinerPanel(class ::Scene& scene);
}

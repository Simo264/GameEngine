#pragma once

#include "Core/Core.hpp"

class Gui
{
public:
	Gui() = default;
	~Gui() = default;

	/**
	 * Setup Dear ImGui context
	 */
	void SetupContext() const;

	/**
	 * Free resources and destroy Dear ImGui context
	 */
	void CleanUp() const;

	/**
	 * Start the Dear ImGui frame
	 */
	void StartFrame() const;

	void Render() const;

	void EndFrame() const;
};

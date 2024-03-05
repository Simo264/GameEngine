#pragma once

#include "Core/Core.hpp"

/* --------------------------------------------------------
	This class takes care of serializing a scene in a file 
	-------------------------------------------------------- */
class SceneSerializer
{
public:
	SceneSerializer() = default;
	
	void SerializeScene(class Scene& scene, const Path& filepath);
	void DeserializeScene(class Scene& scene, const Path& filepath);

private:
	void SerializeComponent(class INIFileParser& conf, const char* section, String& strComponent);
};
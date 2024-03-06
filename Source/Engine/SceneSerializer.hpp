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
	void SerializeObject(class INIFileParser& conf, class GameObject& object);
	void SerializeComponent(
		class INIFileParser& conf, const char* section, const char* componentName, String& strComponent);
};
#pragma once

#include "Core/Core.hpp"

class SceneSerializer
{
public:
	SceneSerializer() = default;
	
	void SerializeScene(class Scene& scene, const fspath& filepath);
	void DeserializeScene(class Scene& scene, const fspath& filepath);

private:
	void SerializeObject(class INIFileParser& conf, class GameObject& object);
	void SerializeComponent(
		class INIFileParser& conf, const char* section, const char* componentName, string& strComponent);
};
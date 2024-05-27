#pragma once

#include "Core/Core.hpp"

class SceneSerializer
{
public:
	SceneSerializer() = default;
	
	void SerializeScene(class Scene& scene, const fspath& filepath);
	void DeserializeScene(class Scene& scene, const fspath& filepath);
};
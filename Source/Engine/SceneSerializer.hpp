#pragma once

#include "Core/Core.hpp"

void SerializeScene(class Scene& scene, const fs::path& filepath);
void DeserializeScene(class Scene& scene, const fs::path& filepath);
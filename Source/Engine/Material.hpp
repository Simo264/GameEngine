#pragma once

#include "Engine/Graphics/Objects/Texture2D.hpp"

struct Material
{
	Texture2D albedo;
	Texture2D normalMap;

	void Bind() const;
};

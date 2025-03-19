#pragma once

#include "Engine/Graphics/Objects/Texture2D.hpp"

class Material
{
public:
	Material() = default;
	~Material() = default;

	Texture2D diffuse;
	Texture2D specular;
	Texture2D normal;

	void Bind(class Program program) const;
};

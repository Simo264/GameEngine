#pragma once

struct Material
{
	class Texture2D* diffuse;
	class Texture2D* specular;

	Material() 
		: diffuse{ nullptr }, 
			specular{ nullptr } 
	{}
};

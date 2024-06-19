#pragma once

struct Material
{
	class Texture2D* diffuse{ nullptr };
	class Texture2D* specular{ nullptr };
	class Texture2D* normal{ nullptr };
	class Texture2D* height{ nullptr };
};

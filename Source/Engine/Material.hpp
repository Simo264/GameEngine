#pragma once

class Texture2D;

struct Material
{
	Texture2D* diffuse;
	Texture2D* specular;
	Texture2D* normal;
	Texture2D* height;
};

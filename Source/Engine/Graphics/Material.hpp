#pragma once

class Texture2D;

struct Material
{
	const Texture2D* diffuse;
	const Texture2D* specular;
	const Texture2D* normal;
};

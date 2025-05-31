#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Base.hpp"

class Transformation
{
public:
	Transformation()
	{
		position = Vec3F(0.f);
		scale = Vec3F(1.f);
		eulerAngles = Vec3F(0.f);
	}
	~Transformation() = default;

	Mat4f GetTransformation();

	Vec3F position;
	Vec3F scale;
	Vec3F eulerAngles;
};
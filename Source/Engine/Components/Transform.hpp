#pragma once

#include "Core/Math/Base.hpp"

namespace Components
{
	struct Transform
	{
		Vec3F position{ 0.f };
		Vec3F scale{ 1.f };
		Vec3F eulerAngles{ 0.f };
		
		Mat4F GetTransformation();
	};
}


#pragma once

#include "ComponentMacros.hpp"
#include "Core/Math/Types.hpp"

namespace Components
{
	DECLARE_COMPONENT(Transform)
	{
		Vec3F position{ 0.f };
		Vec3F scale{ 1.f };
		Vec3F eulerAngles{ 0.f };
		
		Mat4F GetTransformation();
	};
}


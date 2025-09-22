#pragma once

#include "Engine/Graphics/Texture2D.hpp"

namespace Components
{
	struct Material
	{
		Material();
		void Bind() const;

		Texture2D albedo;
		Texture2D normalMap;
	};
}


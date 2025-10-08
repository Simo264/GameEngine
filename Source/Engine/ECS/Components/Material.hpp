#pragma once

#include "ComponentMacros.hpp"
#include "Engine/Graphics/Texture2D.hpp"

namespace Components
{
	DECLARE_COMPONENT(Material)
	{
		Material();
		
		Texture2D albedo;
		Texture2D normalMap;
	};
}


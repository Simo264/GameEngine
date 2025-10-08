#pragma once

#include "ComponentMacros.hpp"
#include "Engine/ECS/Types.hpp"

namespace Components
{
	DECLARE_COMPONENT(AssetIdentifier)
	{
		AssetIdentifier() : assetId{ INVALID_ASSET_ID } {}
		AssetIdentifier(AssetId id) : assetId{ id } {}

		AssetId assetId;
	};
}

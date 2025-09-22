#pragma once

#include "Core/Core.hpp"

using AssetId = u32;
constexpr auto INVALID_ASSET_ID = static_cast<AssetId>(-1);

namespace Components
{
	struct AssetIdentifier
	{
		AssetIdentifier() : assetId{ INVALID_ASSET_ID } {}
		explicit AssetIdentifier(AssetId id) : assetId{ id } {}

		AssetId assetId;
	};
}

#pragma once

#include "Core/Core.hpp"
#include "Core/Singleton.hpp"
#include "Engine/Components/AssetIdentifier.hpp"

class AssetsManager : public Singleton<AssetsManager>
{
public:
	AssetId RegisterAsset(const fs::path& fullPath);
	Optional<const fs::path*> GetAssetPath(AssetId assetId) const;
	AssetId GetAssetId(const fs::path& fullPath) const;
	void Clear();

private:
	UnorderedMap<fs::path, AssetId> _pathToId;
	AssetId _nextAssetId = 0;
};

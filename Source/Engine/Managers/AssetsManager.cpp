#include "AssetsManager.hpp"

AssetId AssetsManager::RegisterAsset(const fs::path& fullPath)
{
  auto it = _pathToId.find(fullPath);
  if (it != _pathToId.end())
    return it->second;

  auto id = _nextAssetId++;
  _pathToId.insert({ fullPath, id });
  return id;
}

Optional<const fs::path*> AssetsManager::GetAssetPath(AssetId assetId) const
{
  for (const auto& [path, id] : _pathToId)
  {
    if (assetId == id)
      return &path;
  }
  return std::nullopt;
}

AssetId AssetsManager::GetAssetId(const fs::path& fullPath) const
{
  auto it = _pathToId.find(fullPath);
  if (it != _pathToId.end())
    return it->second;
  return INVALID_ASSET_ID;
}

void AssetsManager::Clear()
{
	_pathToId.clear();
	_nextAssetId = 0;
}
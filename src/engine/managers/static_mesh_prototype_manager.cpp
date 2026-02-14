#include "StaticMeshPrototypeManager.hpp"

PrototypeData& StaticMeshPrototypeManager::LoadPrototype(AssetId assetId)
{
	auto [data, success] = _prototypes.insert({ assetId, PrototypeData{} });
	if (!success)
		throw std::runtime_error("Error on insert new prototype");

	return data->second;
}

PrototypeData* StaticMeshPrototypeManager::GetPrototype(AssetId assetId)
{
	auto it = _prototypes.find(assetId);
	if (it != _prototypes.end())
		return &it->second;

	return nullptr;
}


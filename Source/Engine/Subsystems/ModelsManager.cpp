#include "ModelsManager.hpp"

#include "Engine/ECS/StaticMesh.hpp"
#include "Core/Log/Logger.hpp"

const StaticMesh* ModelsManager::FindStaticMesh(const fs::path& path) const
{
	auto it = _cacheStaticMesh.find(path);
	if (it != _cacheStaticMesh.end())
		return &it->second;

	return nullptr ;
}

const StaticMesh* ModelsManager::InsertStaticMesh(const fs::path& path)
{
	auto [it, success] = _cacheStaticMesh.emplace(
		std::piecewise_construct,
		std::forward_as_tuple(path),	// Insert key
		std::forward_as_tuple(path)		// construct StaticMesh 
	);
	if (success)
		return &it->second;
	
	CONSOLE_WARN("Error on caching StaticMesh object");
	return nullptr;
}

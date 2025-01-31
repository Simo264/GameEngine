#include "ModelsManager.hpp"

#include "Core/Log/Logger.hpp"
#include "Engine/Filesystem/Filesystem.hpp"

const StaticMesh* ModelsManager::FindStaticMesh(const fs::path& path) const
{
	auto it = _cacheStaticMesh.find(path);
	if (it != _cacheStaticMesh.end())
		return &it->second;

	return nullptr;
}
const StaticMesh* ModelsManager::CreateStaticMesh(const fs::path& path)
{
	CONSOLE_TRACE("Loading static mesh: '{}'...", path.string());
	auto [it, success] = _cacheStaticMesh.emplace(
		std::piecewise_construct,
		std::forward_as_tuple(path.lexically_normal()),	// Insert key
		std::forward_as_tuple()													// Insert empty StaticMesh object
	);
	if (success)
	{
		StaticMesh& mesh = it->second;
		mesh.CreateFromPath(path.lexically_normal());
		return &mesh;
	}
	
	CONSOLE_ERROR("Error on loading static mesh object");
	return nullptr;
}

const SkeletalMesh* ModelsManager::FindSkeletalMesh(const fs::path& path) const
{
	auto it = _cacheSkeletalMesh.find(path);
	if (it != _cacheSkeletalMesh.end())
		return &it->second;

	return nullptr;
}
const SkeletalMesh* ModelsManager::CreateSkeletalMesh(const fs::path& path)
{
	CONSOLE_TRACE("Loading skeletal mesh: '{}'...", path.string());
	auto [it, success] = _cacheSkeletalMesh.emplace(
		std::piecewise_construct,
		std::forward_as_tuple(path.lexically_normal()),	// Insert key
		std::forward_as_tuple()													// construct empty SkeletalMesh object
	);
	if (success)
	{
		SkeletalMesh& skeleton = it->second;
		skeleton.CreateFromFile(path.lexically_normal());
		return &skeleton;
	}

	CONSOLE_WARN("Error on loading SkeletalMesh object");
	return nullptr;
}

void ModelsManager::DestroyAll()
{
	for (auto& [path, staticMesh] : _cacheStaticMesh)
		staticMesh.Destroy();
	for (auto& [path, skeletalMesh] : _cacheSkeletalMesh)
		skeletalMesh.Destroy();
}
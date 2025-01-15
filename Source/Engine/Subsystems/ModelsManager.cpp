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
const StaticMesh* ModelsManager::InsertStaticMesh(const fs::path& path)
{
	CONSOLE_INFO("Loading static mesh object '{}'...", path.string());
	auto [it, success] = _cacheStaticMesh.emplace(
		std::piecewise_construct,
		std::forward_as_tuple(path),	// Insert key
		std::forward_as_tuple()				// Insert empty StaticMesh object
	);
	if (success)
	{
		StaticMesh& mesh = it->second;
		mesh.CreateFromPath(path);
		return &mesh;
	}
	
	CONSOLE_WARN("Error on loading StaticMesh object: '{}'", path.string());
	return nullptr;
}

const SkeletalMesh* ModelsManager::FindSkeletalMesh(const fs::path& path) const
{
	auto it = _cacheSkeletalMesh.find(path);
	if (it != _cacheSkeletalMesh.end())
		return &it->second;

	return nullptr;
}
const SkeletalMesh* ModelsManager::InsertSkeletalMesh(const fs::path& path)
{
	CONSOLE_INFO("Loading skeletal mesh object '{}'...", path.string());
	auto [it, success] = _cacheSkeletalMesh.emplace(
		std::piecewise_construct,
		std::forward_as_tuple(path),	// Insert key
		std::forward_as_tuple()				// construct empty SkeletalMesh object
	);
	if (success)
	{
		SkeletalMesh& skeleton = it->second;
		skeleton.CreateFromFile(path);
		return &skeleton;
	}

	CONSOLE_WARN("Error on loading SkeletalMesh object");
	return nullptr;
}

void ModelsManager::DestroyAll()
{
	for (auto& [path, staticMesh] : _cacheStaticMesh)
		staticMesh.Destroy();

	//for (const auto& [path, skeletalMesh] : _cacheStaticMesh)
	//{
	//	
	//}
}
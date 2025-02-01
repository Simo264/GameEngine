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
	CONSOLE_TRACE("Insert new static mesh: '{}'", path.lexically_normal().string());
	auto [it, success] = _cacheStaticMesh.emplace(
		std::piecewise_construct,
		std::forward_as_tuple(path.lexically_normal()),	// Insert key
		std::forward_as_tuple()													// Insert empty StaticMesh object
	);
	if (!success)
	{
		CONSOLE_ERROR("Error on loading static mesh object");
		return nullptr;
	}

	fs::path absolute = (Filesystem::GetStaticModelsPath() / path).lexically_normal();
	CONSOLE_TRACE("Create new static mesh: '{}'", absolute.string());

	StaticMesh& mesh = it->second;
	mesh.CreateFromPath(absolute);
	return &mesh;
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
	CONSOLE_TRACE("Insert new skeletal mesh: '{}'", path.lexically_normal().string());
	auto [it, success] = _cacheSkeletalMesh.emplace(
		std::piecewise_construct,
		std::forward_as_tuple(path.lexically_normal()),	// Insert key
		std::forward_as_tuple()													// construct empty SkeletalMesh object
	);
	if (!success)
	{
		CONSOLE_WARN("Error on loading SkeletalMesh object");
		return nullptr;
	}
	
	fs::path absolute = (Filesystem::GetSkeletalModelsPath() / path).lexically_normal();
	CONSOLE_TRACE("Create new skeletal mesh: '{}'", absolute.string());

	SkeletalMesh& skeleton = it->second;
	skeleton.CreateFromFile(absolute);
	return &skeleton;
}

void ModelsManager::DestroyAll()
{
	for (auto& [path, staticMesh] : _cacheStaticMesh)
		staticMesh.Destroy();
	for (auto& [path, skeletalMesh] : _cacheSkeletalMesh)
		skeletalMesh.Destroy();
}
#include "ModelsManager.hpp"

#include "Core/Log/Logger.hpp"
#include "Engine/Filesystem/Filesystem.hpp"

const StaticMesh* ModelsManager::FindStaticMesh(const fs::path& relative) const
{
	auto it = _cacheStaticMesh.find(relative);
	if (it != _cacheStaticMesh.end())
		return &it->second;

	return nullptr;
}
const StaticMesh* ModelsManager::CreateStaticMesh(const fs::path& relative)
{
	CONSOLE_TRACE("Insert new static mesh: '{}'", relative.lexically_normal().string());
	auto [it, success] = _cacheStaticMesh.emplace(
		std::piecewise_construct,
		std::forward_as_tuple(relative.lexically_normal()),	// Insert key
		std::forward_as_tuple()															// Insert empty StaticMesh object
	);
	if (!success)
	{
		CONSOLE_ERROR("Error on loading static mesh object");
		return nullptr;
	}

	fs::path absolute = (Filesystem::GetStaticModelsPath() / relative).lexically_normal();
	CONSOLE_TRACE("Create new static mesh: '{}'", absolute.string());

	StaticMesh& mesh = it->second;
	mesh.CreateFromPath(absolute);
	return &mesh;
}

const SkeletalMesh* ModelsManager::FindSkeletalMesh(const fs::path& relative) const
{
	auto it = _cacheSkeletalMesh.find(relative);
	if (it != _cacheSkeletalMesh.end())
		return &it->second;

	return nullptr;
}
const SkeletalMesh* ModelsManager::CreateSkeletalMesh(const fs::path& relative)
{
	CONSOLE_TRACE("Insert new skeletal mesh: '{}'", relative.lexically_normal().string());
	auto [it, success] = _cacheSkeletalMesh.emplace(
		std::piecewise_construct,
		std::forward_as_tuple(relative.lexically_normal()),	// Insert key
		std::forward_as_tuple()													// construct empty SkeletalMesh object
	);
	if (!success)
	{
		CONSOLE_WARN("Error on loading SkeletalMesh object");
		return nullptr;
	}
	
	fs::path absolute = (Filesystem::GetSkeletalModelsPath() / relative).lexically_normal();
	CONSOLE_INFO("Create new skeletal mesh: '{}'", absolute.string());

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
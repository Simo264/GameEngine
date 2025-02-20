#include "ModelsManager.hpp"

#include "Core/Log/Logger.hpp"
#include "Engine/Filesystem/Filesystem.hpp"

const StaticMesh* ModelsManager::FindStaticMesh(const fs::path& relative) const
{
	fs::path normalized = relative.lexically_normal();
	auto it = std::find_if(_staticMeshPaths.begin(), _staticMeshPaths.end(), [&](const std::pair<u32, fs::path>& pair) {
		return pair.second == normalized;
	});
	if (it == _staticMeshPaths.end())
		return nullptr;

	u32 index = it->first;
	return &_staticMeshObjects.at(index);
}
const StaticMesh& ModelsManager::CreateStaticMesh(const fs::path& relative)
{
	fs::path normalized = relative.lexically_normal();
	fs::path absolute = (Filesystem::GetStaticModelsPath() / relative).lexically_normal();

	CONSOLE_TRACE("Create new static mesh: '{}'", absolute.string());
	auto& newMesh = _staticMeshObjects.emplace_back();
	newMesh.CreateFromPath(absolute);
	newMesh.id = _staticMeshObjects.size();

	CONSOLE_TRACE("Insert new static mesh: '{}'", normalized.string());
	_staticMeshPaths.emplace_back(_staticMeshObjects.size() - 1, normalized);

	return newMesh;
}

const SkeletalMesh* ModelsManager::FindSkeletalMesh(const fs::path& relative) const
{
	auto itPath = std::find_if(_skeletalMeshPaths.begin(), _skeletalMeshPaths.end(), [&](const std::pair<u32, fs::path>& pair) {
		return pair.second == relative;
	});
	if (itPath == _skeletalMeshPaths.end())
		return nullptr;

	u32 meshId = itPath->first;
	auto itObj = _skeletalMeshObjects.find(meshId);
	return &itObj->second;
}
const SkeletalMesh* ModelsManager::CreateSkeletalMesh(const fs::path& relative)
{
	fs::path pathNormal = relative.lexically_normal();
	fs::path absolute = (Filesystem::GetSkeletalModelsPath() / pathNormal).lexically_normal();

	CONSOLE_TRACE("Insert new skeletal mesh: '{}'", pathNormal.string());
	u32 skeletonId = _skeletalMeshObjects.size();
	auto [it, success] = _skeletalMeshObjects.emplace(
		std::piecewise_construct,
		std::forward_as_tuple(skeletonId),// Insert key
		std::forward_as_tuple()				// construct empty SkeletalMesh object
	);
	if (!success)
	{
		CONSOLE_WARN("Error on loading SkeletalMesh object");
		return nullptr;
	}
	
	auto& [newID, newPath] = _skeletalMeshPaths.emplace_back(skeletonId, pathNormal);
	
	CONSOLE_INFO("Create new skeletal mesh: '{}'", absolute.string());
	SkeletalMesh& skeleton = it->second;
	skeleton.CreateFromFile(absolute);
	skeleton.id = skeletonId;
	return &skeleton;
}

const fs::path* ModelsManager::GetStaticMeshPath(u32 staticMeshId) const
{
	auto it = std::find_if(_staticMeshPaths.begin(), _staticMeshPaths.end(), [&](const std::pair<u32, fs::path>& pair) {
		return _staticMeshObjects.at(pair.first).id == staticMeshId;
	});
	if (it == _staticMeshPaths.end())
		return nullptr;

	return &it->second;
}
const fs::path* ModelsManager::GetSkeletalMeshPath(u32 skeletalMeshId) const
{
	auto it = std::find_if(_skeletalMeshPaths.begin(), _skeletalMeshPaths.end(), [&](const std::pair<u32, fs::path>& pair) {
		return pair.first == skeletalMeshId;
	});
	if (it != _skeletalMeshPaths.end())
		return &it->second;

	return nullptr;
}

void ModelsManager::DestroyAll()
{
	for (auto& staticMesh : _staticMeshObjects)
		staticMesh.Destroy();

	for (auto& [id, skeletalMesh] : _skeletalMeshObjects)
		skeletalMesh.Destroy();
}
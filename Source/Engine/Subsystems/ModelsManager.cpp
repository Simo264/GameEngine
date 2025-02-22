#include "ModelsManager.hpp"

#include "Core/Log/Logger.hpp"
#include "Engine/Filesystem/Filesystem.hpp"

void ModelsManager::CleanUp()
{
	for (auto& staticMesh : _staticMeshObjects)
		staticMesh.Destroy();

	for (auto& skeletalMesh : _skeletalMeshObjects)
		skeletalMesh.Destroy();
}

const StaticMesh* ModelsManager::FindStaticMesh(const fs::path& relative) const
{
	fs::path normalized = relative.lexically_normal();
	for (u32 i = 0; i < _staticMeshPaths.size(); i++)
		if (_staticMeshPaths.at(i) == normalized)
			return &_staticMeshObjects.at(i);
	return nullptr;
}
const StaticMesh& ModelsManager::CreateStaticMesh(const fs::path& relative)
{
	u32 currentId = _staticMeshObjects.size();
	
	//CONSOLE_INFO("Create new static mesh {}: '{}'", staticMeshId, absolute.string());
	fs::path absolute = (Filesystem::GetStaticModelsPath() / relative).lexically_normal();
	auto& newMesh = _staticMeshObjects.emplace_back();
	newMesh.CreateFromPath(absolute);
	newMesh.id = currentId;

	fs::path normalized = relative.lexically_normal();
	_staticMeshPaths.emplace_back(normalized);
	
	return newMesh;
}

const SkeletalMesh* ModelsManager::FindSkeletalMesh(const fs::path& relative) const
{
	fs::path normalized = relative.lexically_normal();
	for (u32 i = 0; i < _skeletalMeshPaths.size(); i++)
		if (_skeletalMeshPaths.at(i) == normalized)
			return &_skeletalMeshObjects.at(i);
	return nullptr;
}
const SkeletalMesh& ModelsManager::CreateSkeletalMesh(const fs::path& relative)
{
	u32 currentId = _skeletalMeshObjects.size();

	//CONSOLE_INFO("Create new skeletal mesh {}: '{}'", skeletonId, absolute.string());
	fs::path absolute = (Filesystem::GetSkeletalModelsPath() / relative).lexically_normal();
	SkeletalMesh& skeleton = _skeletalMeshObjects.emplace_back();
	skeleton.CreateFromFile(absolute);
	skeleton.id = currentId;

	fs::path normalized = relative.lexically_normal();
	_skeletalMeshPaths.emplace_back(normalized);

	return skeleton;
}

const fs::path* ModelsManager::GetStaticMeshPath(u32 staticMeshId) const
{
	for (u32 i = 0; i < _staticMeshObjects.size(); i++)
		if (_staticMeshObjects.at(i).id == staticMeshId)
			return &_staticMeshPaths.at(i);
	return nullptr;
}

const fs::path* ModelsManager::GetSkeletalMeshPath(u32 skeletalMeshId) const
{
	for (u32 i = 0; i < _skeletalMeshObjects.size(); i++)
		if (_skeletalMeshObjects.at(i).id == skeletalMeshId)
			return &_skeletalMeshPaths.at(i);
	return nullptr;
}

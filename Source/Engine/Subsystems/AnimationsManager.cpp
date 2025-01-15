#include "AnimationsManager.hpp"

#include "Core/Log/Logger.hpp"
#include "Engine/Subsystems/ModelsManager.hpp"


const Vector<Animation>* AnimationsManager::LoadSkeletonAnimations(const fs::path& skeletonPath, const Vector<fs::path> animations)
{
	if (animations.empty())
		CONSOLE_WARN("'{}' does not have animations", skeletonPath.string());
	
	auto [it, success] = _animationsMap.emplace(
		std::piecewise_construct,
		std::forward_as_tuple(skeletonPath),
		std::forward_as_tuple()
	);
	if (success)
	{
		auto& manager = ModelsManager::Get();
		if (const auto* skeleton = manager.FindSkeletalMesh(skeletonPath))
		{
			auto& animVector = it->second;
			animVector.reserve(animations.size());
			for (const auto& path : animations)
				animVector.emplace_back(path, *skeleton);
		}
		else
			CONSOLE_WARN("Can't build animations vector: invalid skeleton");

		return &it->second;
	}
	
	CONSOLE_WARN("Error on loading skeleton's animations '{}'", skeletonPath.string());
	return nullptr;
}
const Vector<Animation>* AnimationsManager::GetSkeletonAnimations(const fs::path& skeletonPath)
{
	auto it = _animationsMap.find(skeletonPath);
	if (it != _animationsMap.end())
		return &it->second;
	
	CONSOLE_WARN("Skeleton '{}' does not exist", skeletonPath.string());
	return nullptr;
}

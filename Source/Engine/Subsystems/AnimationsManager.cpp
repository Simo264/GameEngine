#include "AnimationsManager.hpp"

#include "Core/Log/Logger.hpp"
#include "Engine/Subsystems/ModelsManager.hpp"


const Vector<Animation>* AnimationsManager::LoadSkeletonAnimations(const fs::path& skeletonPath,
																																	 const Vector<fs::path> animationPaths)
{
	CONSOLE_DEBUG("Loading skeleton's animations: {}", skeletonPath.string());

	if (animationPaths.empty())
		CONSOLE_WARN("Current skeleton does not have animations");
	
	auto [it, success] = _animationsMap.emplace(
		std::piecewise_construct,
		std::forward_as_tuple(skeletonPath),
		std::forward_as_tuple()
	);
	if (success)
	{
		auto& manager = ModelsManager::Get();
		const auto* skeleton = manager.FindSkeletalMesh(skeletonPath);
		assert(skeleton != nullptr);
		
		Vector<Animation>& animVector = it->second;
		animVector.reserve(animationPaths.size());
		for (const auto& p : animationPaths)
			animVector.emplace_back(p, *skeleton);

		return &it->second;
	}
	
	CONSOLE_WARN("Error on loading animations");
	return nullptr;
}

const Vector<Animation>* AnimationsManager::FindSkeletonAnimations(const fs::path& skeletonPath)
{
	auto it = _animationsMap.find(skeletonPath);
	if (it != _animationsMap.end())
		return &it->second;
	
	CONSOLE_WARN("Skeleton '{}' does not exist", skeletonPath.string());
	return nullptr;
}

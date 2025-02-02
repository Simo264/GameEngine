#include "AnimationsManager.hpp"

#include "Core/Log/Logger.hpp"
#include "Engine/Subsystems/ModelsManager.hpp"
#include "Engine/Filesystem/Filesystem.hpp"

const Vector<Animation>* AnimationsManager::LoadAnimations(const SkeletalMesh& skeleton,
																													 const fs::path& relativeSkeleton,
																													 const Vector<fs::path> relativeAnims)
{
	CONSOLE_TRACE("Loading animations for {}", relativeSkeleton.string());

	auto [it, success] = _animationsMap.emplace(
		std::piecewise_construct,
		std::forward_as_tuple(relativeSkeleton),
		std::forward_as_tuple()
	);
	if (!success)
	{
		CONSOLE_WARN("Error on loading animations");
		return nullptr;
	}
	
	if (relativeAnims.empty())
	{
		CONSOLE_WARN("Current skeleton does not have animations");
		return &it->second;
	}

	Vector<Animation>& animVector = it->second;
	animVector.reserve(relativeAnims.size());
	for (const auto& p : relativeAnims)
		animVector.emplace_back(skeleton, p);
	return &animVector;
}

const Vector<Animation>* AnimationsManager::GetAnimationsVector(const fs::path& relative)
{
	auto it = _animationsMap.find(relative);
	if (it != _animationsMap.end())
		return &it->second;
	
	return nullptr;
}

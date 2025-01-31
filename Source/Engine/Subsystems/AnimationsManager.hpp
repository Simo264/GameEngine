#pragma once

#include "Core/Core.hpp"
#include "Engine/ECS/Animation/Animation.hpp"

class SkeletalMesh;

class AnimationsManager
{
public:
	/** @brief Deleted copy constructor to enforce singleton pattern. */
	AnimationsManager(const AnimationsManager&) = delete;
	/** @brief Deleted assignment constructor to enforce singleton pattern. */
	void operator=(const AnimationsManager&) = delete;

	/** @return Returns the singleton instance of AnimationsManager. */
	static AnimationsManager& Get()
	{
		static AnimationsManager manager;
		return manager;
	}

	// <skeletonPath> relative path to "Assets/Models/Skeletal"
	// <animationPaths> relative path to "Assets/Models/Skeletal/"
	const Vector<Animation>* LoadSkeletonAnimations(const fs::path& skeletonPath,
																									const Vector<fs::path> animationPaths);
	
	// skeletonPath relative to "Assets/Models/Skeletal"
	const Vector<Animation>* FindSkeletonAnimations(const fs::path& skeletonPath);

private:
	AnimationsManager() = default;
	~AnimationsManager() = default;

	UnorderedMap<fs::path, Vector<Animation>> _animationsMap; // skeleton - animations
};
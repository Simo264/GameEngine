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

	// <relativeSkeleton> the relative path to the skeletal mesh object "Models/Skeletal"
	// E.g. "Character/Character.gltf"
	// <relativeAnims> the relative path to the skeletal mesh animations
	// E.g. [ 
	//	"Drunk_Walk/anim.gltf", 
	//	"Silly_Dancing/anim.gltf" 
	// ]
	const Vector<Animation>* LoadAnimations(const SkeletalMesh& skeleton, 
																					const fs::path& relativeSkeleton,
																					const Vector<fs::path> relativeAnims);
	
	// <relative> is the reltive path to skeletal mesh object.
	// E.g. "Character/Character.gltf"
	const Vector<Animation>* GetAnimationsVector(const fs::path& relative);

private:
	AnimationsManager() = default;
	~AnimationsManager() = default;
	
	// skeleton's relative path - animations
	// E.g "Mutant/Mutant.gltf" - [ "Drunk_Walk/anim.gltf", "Silly_Dancing/anim.gltf" ]
	UnorderedMap<fs::path, Vector<Animation>> _animationsMap;
};
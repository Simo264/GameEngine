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

	// <animationPaths> list of relative paths to "Assets/Models/Skeletal/<name>"
	// E.g:
	// Skeleton.path = "GameEngine/Assets/Models/Skeletal/Mutant/Mutant.gltf" 
	// animationPaths = { 
	//	"Drunk_Walk/anim.gltf", 
	//	"Silly_Dancing/anim.gltf" 
	// }
	const Vector<Animation>* LoadAnimations(const SkeletalMesh& skeleton,
																					const Vector<fs::path> animationPaths);
	
	const Vector<Animation>* GetAnimationsVector(const SkeletalMesh& skeleton);

private:
	AnimationsManager() = default;
	~AnimationsManager() = default;
	
	// skeleton's relative path - animations
	// E.g "Mutant/" - Vector{ "Drunk_Walk", "Silly_Dancing" }
	UnorderedMap<fs::path, Vector<Animation>> _animationsMap; 
};
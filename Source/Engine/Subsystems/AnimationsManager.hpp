#pragma once

#include "Core/Core.hpp"
#include "Engine/ECS/Animation/Animation.hpp"

class SkeletalMesh;

class AnimationsManager
{
public:
	AnimationsManager(const AnimationsManager&) = delete;
	void operator=(const AnimationsManager&) = delete;

	static AnimationsManager& Get()
	{
		static AnimationsManager manager;
		return manager;
	}

	/**
	 * @brief Loads animations for a given skeletal mesh.
	 * This method loads multiple animations associated with a specific skeletal mesh,
	 * using the provided relative paths.
	 *
	 * @param skeleton The skeletal mesh to which the animations belong.
	 * @param relativeAnims A vector of relative paths to the animation files inside the skeletal model directory.
	 *        Example: { "Mutant/Drunk_Walk/anim.gltf", "Mutant/Silly_Dancing/anim.gltf" }.
	 * 
	 * @return Pointer to a vector of loaded Animation objects.
	 */
	const Vector<Animation>* LoadAnimations(const SkeletalMesh& skeleton,
																					const Vector<fs::path> relativeAnims);

	/**
	 * @brief Retrieves the vector of animations associated with a skeletal mesh.
	 * This method returns a pointer to a vector containing all animations that belong to the specified skeletal mesh.
	 *
	 * @return Pointer to a vector of Animation objects, or nullptr if no animations are found.
	 */
	const Vector<Animation>* GetSkeletonAnimations(u32 skeletonID);

	const fs::path* GetAnimationPath(u32 animationID) const;

	void CleanUp();

private:
	AnimationsManager() = default;
	~AnimationsManager() = default;
	
	// Skeleton id + list of animations
	Map<u32, Vector<Animation>> _skeletonAnimations;

	// Animation id + it's path
	Vector<std::pair<u32, fs::path>> _animationPaths;
};
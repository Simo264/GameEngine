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

	/**
	 * @brief Loads animations for a given skeletal mesh.
	 * This method loads multiple animations associated with a specific skeletal mesh,
	 * using the provided relative paths.
	 *
	 * @param skeleton The skeletal mesh to which the animations belong.
	 * @param relativeSkeleton The relative path to the skeletal mesh inside "Models/Skeletal".
	 *				Example: "Character/Character.gltf".
	 * @param relativeAnims A vector of relative paths to the animation files inside the skeletal model directory.
	 *        Example: { "Drunk_Walk/anim.gltf", "Silly_Dancing/anim.gltf" }.
	 * @return Pointer to a vector of loaded Animation objects.
	 */
	const Vector<Animation>* LoadAnimations(const SkeletalMesh& skeleton,
																					const fs::path& relativeSkeleton,
																					const Vector<fs::path> relativeAnims);

	/**
	 * @brief Retrieves the vector of animations associated with a skeletal mesh.
	 * This method returns a pointer to a vector containing all animations that belong to the specified skeletal mesh.
	 *
	 * @param relative The relative path to the skeletal mesh inside "Models/Skeletal".
	 *        Example: "Character/Character.gltf".
	 * @return Pointer to a vector of Animation objects, or nullptr if no animations are found.
	 */
	const Vector<Animation>* GetAnimationsVector(const fs::path& relative);

private:
	AnimationsManager() = default;
	~AnimationsManager() = default;
	
	// skeleton's relative path - animations
	// E.g "Mutant/Mutant.gltf" - [ "Drunk_Walk/anim.gltf", "Silly_Dancing/anim.gltf" ]
	UnorderedMap<fs::path, Vector<Animation>> _animationsMap;
};
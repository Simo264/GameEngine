#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Base.hpp"

/** @brief Represents a single node in a skeleton hierarchy. */
struct BoneNode
{
	/** @brief Name of the bone. */
	char name[32]{};

	/**
	 * @brief Initial bind pose transformation.
	 *
	 * This matrix represents the bone's transformation in the initial bind pose (rest position).
	 * It is used to reset or reference the bone's original state when needed, ensuring
	 * accurate blending between animations or returning to the default pose.
	 */
	mat4f bindPoseTransform{};
	
	/** @brief Index of the bone. */
	i32 boneIndex{ -1 };
	
	/** @brief List of child nodes. */
	Vector<BoneNode> children{};
};

/**
 * @struct Bone
 * @brief Represents a single bone in the skeleton hierarchy.
 *
 * The Bone structure stores essential transformation data for each bone in a skeletal system.
 * It contains information about the bone's offset matrix, current local transformation,
 * and its bind pose transformation. This data is crucial for skeletal animations, allowing
 * the bone to transform and animate models efficiently.
 */
struct Bone
{
	/**
	 * @brief Offset matrix for the bone.
	 *
	 * The offset matrix is a static matrix that transforms vertices from model space to
	 * the bone's local space. It is precomputed during the rigging process and remains
	 * constant throughout the animation. This matrix is vital for skinning and vertex
	 * transformations.
	 */
	mat4f offset{};

	/**
	 * @brief Current local transformation of the bone.
	 *
	 * Represents the bone's position, rotation, and scale relative to its parent bone during
	 * an animation. This matrix is updated dynamically as the animation progresses,
	 * reflecting the bone's current pose.
	 */
	mat4f	localTransform{};
};
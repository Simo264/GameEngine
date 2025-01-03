#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Core.hpp"

/**
 * @brief Represents a single node in a skeleton hierarchy.
 *
 * This structure is used to define the hierarchical relationship between bones in a skeleton.
 * Each node has a local transformation relative to its parent and may have child nodes.
 */
struct BoneNode
{
	/**
	 * @brief Name of the bone.
	 *
	 * This field stores the name of the bone as a null-terminated string. It is used for identification
	 * purposes within a bone hierarchy, typically allowing the bone to be referenced in animations
	 * or when manipulating the skeleton structure.
	 */
	char name[32]{};

	/**
	 * @brief Local transformation matrix.
	 *
	 * Represents the position, rotation, and scale of this bone relative to its parent in the hierarchy.
	 */
	mat4f transformation{};
	
	/**
	 * @brief Index of the bone.
	 *
	 * Used to identify the bone
	 */
	u32 boneIndex{ static_cast<u32>(-1) };
	
	/**
	 * @brief List of child nodes.
	 *
	 * Represents the bones connected to this bone. Each child defines its own local transformation.
	 */
	Vector<BoneNode> children{};
};

/**
 * @brief Represents a single bone in the skeleton with transformation data.
 *
 * This structure is used for storing bone-specific data such as its offset matrix and its current transformation.
 */
struct Bone
{
	/**
	 * @brief Offset matrix for the bone.
	 *
	 * This matrix transforms vertices from the model's space to the bone's local space.
	 * It is typically static and precomputed based on the bind pose of the skeleton.
	 */
	mat4f offset{};

	/**
	 * @brief Local transformation matrix.
	 *
	 * Represents the current position, rotation, and scale of the bone in the skeleton during animation.
	 */
	mat4f	localTransform{};
};
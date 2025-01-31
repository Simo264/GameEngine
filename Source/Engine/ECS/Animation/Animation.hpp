#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Base.hpp"

class SkeletalMesh;

struct aiAnimation;
struct aiNodeAnim;
struct aiNode;

struct KeyPosition
{
	f32 timeStamp{};
	vec3f position{};
};
struct KeyRotation
{
	f32 timeStamp{};
	quat orientation{};
};
struct KeyScale
{
	f32 timeStamp{};
	vec3f scale{};
};
struct AnimationKeys
{
	i32 boneIndex{};
	Vector<KeyPosition> positionKeys{};
	Vector<KeyRotation> rotationKeys{};
	Vector<KeyScale>		scaleKeys{};
};

/**
 * @class Animation
 * @brief Represents an animation associated with a skeleton mesh.
 */
class Animation
{
public:
	// <path> relative path to "Assets/Models/Skeletal/"
	Animation(const fs::path& path,
						const SkeletalMesh& skeleton);
	
	/** @brief Default destructor for the Animation class. */
	~Animation() = default;

	const auto& BoneKeys() const { return _boneKeys; }

	fs::path path;
	f32 duration;
	f32 ticksPerSecond;

private:
	void LoadAnimation(const aiAnimation* animation, const SkeletalMesh& skeleton);
	void LoadBoneKeys(AnimationKeys& keys, const aiNodeAnim* channel);

	Vector<AnimationKeys> _boneKeys;
};
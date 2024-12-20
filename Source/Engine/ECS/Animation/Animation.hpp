#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Core.hpp"
#include "Engine/ECS/Skeleton/SkeletonMesh.hpp"

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
	Vector<KeyPosition> positionKeys{};
	Vector<KeyRotation> rotationKeys{};
	Vector<KeyScale>		scaleKeys{};
};

class Animation
{
public:
	Animation(const fs::path& path, SkeletonMesh& skeleton);
	~Animation() = default;

	f32 duration;
	f32 ticksPerSecond;
	Map<String, AnimationKeys> boneKeys; // BoneName - KeyFrames

private:
	void LoadAnimation(const aiAnimation* animation, SkeletonMesh& skeleton);
	void LoadSkeletonBone(SkeletonMesh& skeleton, StringView boneName);
	void LoadBoneKeys(AnimationKeys& keys, const aiNodeAnim* channel);
};
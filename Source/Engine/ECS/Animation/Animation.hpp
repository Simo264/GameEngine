#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Core.hpp"
#include "Engine/ECS/Skeleton/SkeletonMesh.hpp"

struct aiAnimation;
struct aiNode;

class Animation
{
public:
	Animation(const fs::path& path, SkeletonMesh& skeleton);
	~Animation() = default;

	Bone* FindBone(StringView name);

	BoneNode rootNode;
	f32 duration;
	f32 ticksPerSecond;
	Vector<Bone> bones;
	UnorderedMap<String, BoneInfo>* boneMap;

private:
	void ReadMissingBones(const aiAnimation* animation, SkeletonMesh& skeleton);
	void ReadHierarchyData(BoneNode& dest, const aiNode* src);
};
#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Core.hpp"
#include "Engine/ECS/Skeleton/Skeleton.hpp"

struct aiAnimation;
struct aiNode;

struct AssimpNodeData
{
	mat4f transformation{};
	String name{};
	Vector<AssimpNodeData> children{};
	u32 childrenCount{};
};

class Animation
{
public:
	Animation() = default;
	Animation(const fs::path& path, Skeleton& skeleton);
	~Animation() = default;

	Bone* FindBone(StringView name);

	f32 GetTicksPerSecond() const { return _ticksPerSecond; }
	f32 GetDuration() const { return _duration; }
	const AssimpNodeData& GetRootNode() { return _rootNode; }
	const auto* GetBoneInfoMap() { return _boneInfoMap; }

private:
	void ReadMissingBones(const aiAnimation* animation, Skeleton& skeleton);
	void ReadHierarchyData(AssimpNodeData& dest, const aiNode* src);

	AssimpNodeData _rootNode;
	f32 _duration;
	f32 _ticksPerSecond;
	Vector<Bone> _bones;
	UnorderedMap<String, BoneInfo>* _boneInfoMap;
};
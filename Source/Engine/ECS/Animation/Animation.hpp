#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Core.hpp"
#include "Engine/ECS/Skeleton/Skeleton.hpp"

struct aiAnimation;
struct aiNode;

struct AssimpNodeData
{
	mat4f transformation;
	String name;
	i32 childrenCount;
	Vector<AssimpNodeData> children;
};

class Animation
{
public:
	Animation() = default;
	Animation(const fs::path& path, Skeleton& skeleton);
	~Animation() = default;

	Bone* FindBone(const String& name);

	float GetTicksPerSecond() const { return m_TicksPerSecond; }
	float GetDuration() const { return m_Duration; }
	const AssimpNodeData& GetRootNode() { return m_RootNode; }
	const Map<String, BoneInfo>& GetBoneIDMap() { return m_BoneInfoMap; }

private:
	void ReadMissingBones(const aiAnimation* animation, Skeleton& skeleton);
	void ReadHierarchyData(AssimpNodeData& dest, const aiNode* src);


	f32 m_Duration;
	i32 m_TicksPerSecond;
	Vector<Bone> m_Bones;
	AssimpNodeData m_RootNode;
	Map<String, BoneInfo> m_BoneInfoMap;
};
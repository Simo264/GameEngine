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
  Animation(const fs::path& animationPath, Skeleton& skeletal);
	~Animation() = default;

  Bone* FindBone(StringView name);

  f32 GetDuration() const { return _duration; }
  i32 GetTicksPerSecond() const { return _ticksPerSecond; }
  const AssimpNodeData& GetRootNode() const { return _rootNode; }
  const auto& GetBoneInfoMap() const { return _boneInfoMap; }

private:
  void ReadMissingBones(const aiAnimation* animation, Skeleton& skeletal);
  void ReadHeirarchyData(AssimpNodeData& dest, const aiNode* src);

  f32 _duration;
  i32 _ticksPerSecond;

  Vector<Bone> _bones;
  AssimpNodeData _rootNode;
  UnorderedMap<String, BoneInfo> _boneInfoMap;
};
#include "Animation.hpp"

#include "Core/Log/Logger.hpp"
#include "Engine/Filesystem/Filesystem.hpp"
#include "Engine/ECS/Skeleton/SkeletalMesh.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// ---------------------------------------------------- 
// 									PUBLIC														
// ---------------------------------------------------- 

Animation::Animation(const fs::path& path, const SkeletalMesh& skeleton) :
	path{ path },
	duration{ 0 },
	ticksPerSecond{ 0 },
	_boneKeys{}
{
	fs::path absolutePath = Filesystem::GetSkeletalModelsPath() / path;
	
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(absolutePath.string(), aiProcess_Triangulate);
	if (!scene || !scene->mRootNode)
	{
		CONSOLE_ERROR("Assimp importer error: {}", importer.GetErrorString());
		return;
	}
	if (!scene->HasAnimations())
	{
		CONSOLE_ERROR("Error: '{}' !scene->HasAnimations() ", path.string());
		return;
	}

	aiAnimation* animation = scene->mAnimations[0];
	duration = animation->mDuration;
	ticksPerSecond = animation->mTicksPerSecond;
	_boneKeys.reserve(animation->mNumChannels);
	LoadAnimation(animation, skeleton);

	std::sort(_boneKeys.begin(), _boneKeys.end(), [](const AnimationKeys& a, const AnimationKeys& b) {
		return a.boneIndex < b.boneIndex;
	});
}

// ----------------------------------------------------
//										PRIVATE													
// ----------------------------------------------------

void Animation::LoadAnimation(const aiAnimation* animation, const SkeletalMesh& skeleton)
{
	for (i32 i = 0; i < animation->mNumChannels; i++)
	{
		aiNodeAnim* channel = animation->mChannels[i];
		const char* boneName = channel->mNodeName.data;

		// Load skeleton bones
		auto [bone, boneIndex] = skeleton.FindBone(boneName);
		if (!bone)
			continue;

		// Load bone keys
		AnimationKeys& keys = _boneKeys.emplace_back();
		keys.boneIndex = boneIndex;
		keys.positionKeys.reserve(channel->mNumPositionKeys);
		keys.rotationKeys.reserve(channel->mNumRotationKeys);
		keys.scaleKeys.reserve(channel->mNumScalingKeys);
		LoadBoneKeys(keys, channel);
	}
}
void Animation::LoadBoneKeys(AnimationKeys& keys, const aiNodeAnim* channel)
{
	for (u32 i = 0; i < channel->mNumPositionKeys; i++)
	{
		f32 timestamp = channel->mPositionKeys[i].mTime;
		vec3f position = vec3f(
			channel->mPositionKeys[i].mValue.x,
			channel->mPositionKeys[i].mValue.y,
			channel->mPositionKeys[i].mValue.z
		);
		auto& key = keys.positionKeys.emplace_back();
		key.timeStamp = timestamp;
		key.position = position;
	}
	for (u32 i = 0; i < channel->mNumRotationKeys; i++)
	{
		f32 timestamp = channel->mRotationKeys[i].mTime;
		quat orientation = quat(
			channel->mRotationKeys[i].mValue.w,
			channel->mRotationKeys[i].mValue.x,
			channel->mRotationKeys[i].mValue.y,
			channel->mRotationKeys[i].mValue.z
		);
		auto& key = keys.rotationKeys.emplace_back();
		key.timeStamp = timestamp;
		key.orientation = orientation;
	}
	for (u32 i = 0; i < channel->mNumScalingKeys; i++)
	{
		f32 timestamp = channel->mScalingKeys[i].mTime;
		vec3f scale = vec3f(
			channel->mScalingKeys[i].mValue.x,
			channel->mScalingKeys[i].mValue.y,
			channel->mScalingKeys[i].mValue.z
		);
		auto& key = keys.scaleKeys.emplace_back();
		key.timeStamp = timestamp;
		key.scale = scale;
	}
}

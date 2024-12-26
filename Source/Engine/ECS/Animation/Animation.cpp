#include "Animation.hpp"

#include "Core/Log/Logger.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

/* ---------------------------------------------------- */
/*										PUBLIC														*/
/* ---------------------------------------------------- */

Animation::Animation(const fs::path& path, SkeletonMesh& skeleton) :
	_path{ path },
	_duration{ 0 },
	_ticksPerSecond{ 0 },
	_boneKeys{}
{
	CONSOLE_INFO("Loading animation '{}'", path.string());

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path.string(), aiProcess_Triangulate);
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
	_duration = animation->mDuration;
	_ticksPerSecond = animation->mTicksPerSecond;
	_boneKeys.reserve(animation->mNumChannels);
	LoadAnimation(animation, skeleton);

	std::sort(_boneKeys.begin(), _boneKeys.end(), [](const AnimationKeys& a, const AnimationKeys& b) {
		return a.boneIndex < b.boneIndex;
	});
}

/* ---------------------------------------------------- */
/*										PRIVATE														*/
/* ---------------------------------------------------- */

void Animation::LoadAnimation(const aiAnimation* animation, SkeletonMesh& skeleton)
{
	for (i32 i = 0; i < animation->mNumChannels; i++)
	{
		aiNodeAnim* channel = animation->mChannels[i];
		const char* boneName = channel->mNodeName.data;

		u32 boneIndex{};

		/* Load skeleton bones */
		auto [bone, index] = skeleton.FindBone(boneName);
		if (!bone)
		{
			auto [newBone, newIndex] = skeleton.InsertBone(boneName);
			boneIndex = newIndex;
		}
		else
			boneIndex = index;

		/* Load bone keys */
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

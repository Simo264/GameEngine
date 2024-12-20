#include "Animation.hpp"

#include "Core/Log/Logger.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

static mat4f AiMatrixToGLM(const aiMatrix4x4& from)
{
	mat4f to{};
	//the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
	to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
	to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
	to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
	to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
	return to;
}

/* ---------------------------------------------------- */
/*										PUBLIC														*/
/* ---------------------------------------------------- */

Animation::Animation(const fs::path& path, SkeletonMesh& skeleton) :
	duration{ 0 },
	ticksPerSecond{ 0 },
	boneKeys{}
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path.string(), aiProcess_Triangulate);
	if (!scene || !scene->mRootNode || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
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
	LoadAnimation(animation, skeleton);
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

		/* Load skeleton bones */
		auto [bone, index] = skeleton.FindBone(boneName);
		if (!bone)
			LoadSkeletonBone(skeleton, boneName);


		/* Load bone keys */
		auto [boneKeysIt, success] = boneKeys.insert({ boneName, AnimationKeys() });
		AnimationKeys& keys = boneKeysIt->second;
		keys.positionKeys.reserve(channel->mNumPositionKeys);
		keys.rotationKeys.reserve(channel->mNumRotationKeys);
		keys.scaleKeys.reserve(channel->mNumScalingKeys);
		LoadBoneKeys(keys, channel);
	}
}
void Animation::LoadSkeletonBone(SkeletonMesh& skeleton, StringView boneName)
{
	u32 boneIndex = skeleton.bones.size();
	skeleton.boneMap.insert({ boneName.data(), boneIndex});
	auto& bone = skeleton.bones.emplace_back();
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

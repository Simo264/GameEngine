#include "Animation.hpp"

#include "Core/Log/Logger.hpp"
#include "Engine/Filesystem/Filesystem.hpp"
#include "Engine/ECS/Skeleton/SkeletalMesh.hpp"
#include "Engine/Subsystems/ModelsManager.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>



// ---------------------------------------------------- 
// 									PUBLIC														
// ---------------------------------------------------- 

Animation::Animation(const SkeletalMesh& skeleton, const fs::path& relative) :
	bonesAnimKeys{},
	nrKeys{ 0 },
	duration{ 0 },
	ticksPerSecond{ 0 },
	id{}
{
	// E.g. relative = "Mutant/Drunk_Walk/anim.gltf"
	// E.g. absolute = "D:GameEngine/Assets/Models/Skeletal/Mutant/Drunk_Walk/anim.gltf"
	fs::path absolute = (Filesystem::GetSkeletalModelsPath() / relative);

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(absolute.string(), 
																					 aiProcess_Triangulate |
																					 aiProcess_LimitBoneWeights |
																					 aiProcess_JoinIdenticalVertices);
	if (!scene || !scene->mRootNode)
	{
		CONSOLE_ERROR("Assimp importer error: {}", importer.GetErrorString());
		return;
	}
	if (!scene->HasAnimations())
	{
		CONSOLE_ERROR("Error: '{}' !scene->HasAnimations() ", absolute.string());
		return;
	}

	aiAnimation* animation = scene->mAnimations[0];
	duration = animation->mDuration;
	ticksPerSecond = animation->mTicksPerSecond;

	bonesAnimKeys = std::make_unique<BoneAnimationKeys[]>(skeleton.nrBones);
	nrKeys = skeleton.nrBones;
	
	for (u32 i = 0; i < animation->mNumChannels; i++)
	{
		aiNodeAnim* channel = animation->mChannels[i];
		const char* channelName = channel->mNodeName.C_Str();

		i32 boneIndex = skeleton.FindBone(channelName);
		if (boneIndex == -1)
			continue;

		auto& boneKeys = bonesAnimKeys[boneIndex];
		LoadBoneKeys(boneKeys, channel);
	}
}

// ----------------------------------------------------
//										PRIVATE													
// ----------------------------------------------------

void Animation::LoadBoneKeys(BoneAnimationKeys& boneKeys, const aiNodeAnim* channel)
{
	boneKeys.posKeys = std::make_unique<KeyPosition[]>(channel->mNumPositionKeys);
	boneKeys.nrPosKeys = channel->mNumPositionKeys;
	for (u32 i = 0; i < channel->mNumPositionKeys; i++)
	{
		aiVectorKey& tmp = channel->mPositionKeys[i];
		boneKeys.posKeys[i].timeStamp = tmp.mTime;
		boneKeys.posKeys[i].position = vec3f(tmp.mValue.x, tmp.mValue.y, tmp.mValue.z);
	}

	boneKeys.rotKeys = std::make_unique<KeyRotation[]>(channel->mNumRotationKeys);
	boneKeys.nrRotKeys = channel->mNumRotationKeys;
	for (u32 i = 0; i < channel->mNumRotationKeys; i++)
	{
		aiQuatKey& tmp = channel->mRotationKeys[i];
		boneKeys.rotKeys[i].timeStamp = tmp.mTime;
		boneKeys.rotKeys[i].orientation = quat(tmp.mValue.w, tmp.mValue.x, tmp.mValue.y, tmp.mValue.z);
	}

	boneKeys.scaleKeys = std::make_unique<KeyScale[]>(channel->mNumScalingKeys);
	boneKeys.nrScaleKeys = channel->mNumScalingKeys;
	for (u32 i = 0; i < channel->mNumScalingKeys; i++)
	{
		aiVectorKey& tmp = channel->mScalingKeys[i];
		boneKeys.scaleKeys[i].timeStamp = tmp.mTime;
		boneKeys.scaleKeys[i].scale = vec3f(tmp.mValue.x, tmp.mValue.y, tmp.mValue.z);
	}
}

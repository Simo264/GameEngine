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

Animation::Animation(const fs::path& path, Skeleton& skeleton) : 
	_duration{ 0 },
	_ticksPerSecond{ 0 },
	_rootNode{},
	_bones{},
	_boneInfoMap{}
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path.string(), aiProcess_Triangulate);
	if (!scene || !scene->mRootNode || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
	{
		CONSOLE_ERROR("Assimp importer error: {}", importer.GetErrorString());
		return;
	}
	
	aiAnimation* animation = scene->mAnimations[0];
	_duration = animation->mDuration;
	_ticksPerSecond = animation->mTicksPerSecond;
	
	ReadHierarchyData(_rootNode, scene->mRootNode);
	ReadMissingBones(animation, skeleton);
}
Bone* Animation::FindBone(StringView name)
{
	auto it = std::find_if(_bones.begin(), _bones.end(), 
		[&](const Bone& Bone) { return Bone.GetBoneName().compare(name.data()) == 0; }
	);
	if (it == _bones.end())
		return nullptr;
	
	return &(*it);
}


/* ---------------------------------------------------- */
/*										PRIVATE														*/
/* ---------------------------------------------------- */

void Animation::ReadMissingBones(const aiAnimation* animation, Skeleton& skeleton)
{
	auto& boneInfoMap = skeleton.GetBoneInfoMap();
	u32& boneCount = skeleton.GetBoneCount();

	u32 size = animation->mNumChannels;
	for (i32 i = 0; i < size; i++)
	{
		aiNodeAnim* channel = animation->mChannels[i];
		const char* boneName = channel->mNodeName.data;

		i32 boneID = boneCount;
		auto it = boneInfoMap.find(boneName);
		if (it == boneInfoMap.end())
		{
			boneInfoMap.emplace(boneName, BoneInfo(boneCount, mat4f(1.0f)));
			boneCount++;
		}
		else
			boneID = it->second.id;
		
		_bones.emplace_back(boneName, boneID, channel);
	}

	_boneInfoMap = &boneInfoMap;
}
void Animation::ReadHierarchyData(AssimpNodeData& dest, const aiNode* src)
{
	dest.name = src->mName.data;
	dest.transformation = AiMatrixToGLM(src->mTransformation);
	dest.childrenCount = src->mNumChildren;
	for (u32 i = 0; i < src->mNumChildren; i++)
	{
		AssimpNodeData newData;
		ReadHierarchyData(newData, src->mChildren[i]);
		dest.children.push_back(newData);
	}
}
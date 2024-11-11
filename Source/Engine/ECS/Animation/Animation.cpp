#include "Animation.hpp"

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

Animation::Animation(const fs::path& animationPath, Skeleton& skeletal)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(animationPath.string(), aiProcess_Triangulate);

	const aiAnimation* animation = scene->mAnimations[0];
	_duration = animation->mDuration;
	_ticksPerSecond = animation->mTicksPerSecond;
	ReadHeirarchyData(_rootNode, scene->mRootNode);
	ReadMissingBones(animation, skeletal);
}

Bone* Animation::FindBone(StringView name)
{
	static auto f = [&](const Bone& bone) { return bone.GetName() == name; };
	auto it = std::find_if(_bones.begin(), _bones.end(), f);
	if (it != _bones.end())
		return &(*it);

	return nullptr;
}

/* ---------------------------------------------------- */
/*										PRIVATE														*/
/* ---------------------------------------------------- */

void Animation::ReadMissingBones(const aiAnimation* animation, Skeleton& skeletal)
{
	auto& boneInfoMap = skeletal.GetBoneInfoMap();/* getting the BoneInfoMap from Skeletal */ 
	i32& boneCount = skeletal.GetBoneCount();			/* getting the BoneCounter from Skeletal */

	/* Reading channels (bones engaged in an animation and their keyframes) */
	for (i32 i = 0; i < animation->mNumChannels; i++)
	{
		const aiNodeAnim* channel = animation->mChannels[i];
		const char* boneName = channel->mNodeName.data;

		i32 boneId = 0;
		auto it = boneInfoMap.find(boneName);
		if (it == boneInfoMap.end())
		{
			boneId = boneCount;
			boneCount++;
			boneInfoMap.emplace(boneName, BoneInfo(boneId, mat4f(1.0f)));
		}
		else
			boneId = it->second.id;

		_bones.emplace_back(boneName, boneId, channel);
	}
	_boneInfoMap = boneInfoMap;
}
void Animation::ReadHeirarchyData(AssimpNodeData& dest, const aiNode* src)
{
	dest.name = src->mName.data;
	dest.transformation = AiMatrixToGLM(src->mTransformation);
	dest.childrenCount = src->mNumChildren;

	for (i32 i = 0; i < src->mNumChildren; i++)
	{
		AssimpNodeData newData;
		ReadHeirarchyData(newData, src->mChildren[i]);
		dest.children.push_back(newData);
	}
}

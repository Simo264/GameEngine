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
	rootNode{},
	bones{},
	boneMap{}
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

	ReadHierarchyData(rootNode, scene->mRootNode);
	ReadMissingBones(animation, skeleton);
}
Bone* Animation::FindBone(StringView name)
{
	auto it = std::find_if(bones.begin(), bones.end(), [&](const Bone& Bone) { 
		return Bone.GetBoneName().compare(name.data()) == 0;
	});
	
	if (it != bones.end())
		return &(*it);

	return nullptr;

	//auto it = boneMap->find(name.data());
	//if (it != boneMap->end())
	//{
	//	BoneInfo& info = it->second;
	//	return &bones.at(info.id);
	//}
	//return nullptr;
}


/* ---------------------------------------------------- */
/*										PRIVATE														*/
/* ---------------------------------------------------- */

void Animation::ReadMissingBones(const aiAnimation* animation, SkeletonMesh& skeleton)
{
	for (i32 i = 0; i < animation->mNumChannels; i++)
	{
		aiNodeAnim* channel = animation->mChannels[i];
		const char* boneName = channel->mNodeName.data;

		i32 boneId{};
		auto it = skeleton.boneMap.find(boneName);
		if (it == skeleton.boneMap.end())
		{
			boneId = skeleton.boneMap.size();
			skeleton.boneMap.emplace(
				std::piecewise_construct,
				std::forward_as_tuple(boneName),
				std::forward_as_tuple(boneId, mat4f(1.0f))
			);
		}
		else
			boneId = it->second.id;

		bones.emplace_back(boneName, boneId, channel);
	}

	boneMap = &skeleton.boneMap;
}
void Animation::ReadHierarchyData(BoneNode& dest, const aiNode* src)
{
	dest.name = src->mName.data;
	dest.transformation = AiMatrixToGLM(src->mTransformation);
	dest.children.reserve(src->mNumChildren);
	for (u32 i = 0; i < src->mNumChildren; i++)
	{
		BoneNode& child = dest.children.emplace_back();
		ReadHierarchyData(child, src->mChildren[i]);
	}
}

#include "AnimationsManager.hpp"

#include "Core/Log/Logger.hpp"
#include "Engine/Subsystems/ModelsManager.hpp"
#include "Engine/Filesystem/Filesystem.hpp"



const Vector<Animation>& AnimationsManager::LoadAnimations(const SkeletalMesh& skeleton,
																													 const Vector<fs::path> relativeAnims)
{
	static u32 animationId = 0;

	// E.g. relativeAnims = [ 
	//	"Mutant/Drunk_Walk/anim.gltf", 
	//	"Mutant/Silly_Dancing/anim.gltf" 
	// ]

	CONSOLE_INFO("Loading animations for skeleton {}", skeleton.id);

	auto [it, success] = _skeletonAnimations.emplace(skeleton.id, Vector<Animation>{});
	assert(success);

	Vector<Animation>& animVector = it->second;
	animVector.reserve(relativeAnims.size());
	for (const auto& relative : relativeAnims)
	{
		auto& animation = animVector.emplace_back(skeleton, relative);
		animation.id = animationId;
		
		_animationPaths.emplace(animationId, relative);
		animationId++;
	}
	return animVector;
}

const Vector<Animation>* AnimationsManager::GetSkeletonAnimations(u32 skeletonID)
{
	auto it = _skeletonAnimations.find(skeletonID);
	if (it != _skeletonAnimations.end())
		return &it->second;
	
	return nullptr;
}

const fs::path* AnimationsManager::GetAnimationPath(u32 animationID) const
{
	auto it = _skeletonAnimations.find(animationID);
	if(it != _skeletonAnimations.end())
		return &_animationPaths.at(animationID);
	return nullptr;
}

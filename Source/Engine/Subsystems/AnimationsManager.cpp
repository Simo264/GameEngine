#include "AnimationsManager.hpp"

#include "Core/Log/Logger.hpp"
#include "Engine/Subsystems/ModelsManager.hpp"
#include "Engine/Filesystem/Filesystem.hpp"

static u32 animationId = 0;

const Vector<Animation>* AnimationsManager::LoadAnimations(const SkeletalMesh& skeleton,
																													 const Vector<fs::path> relativeAnims)
{
	// E.g. relativeAnims = [ 
	//	"Mutant/Drunk_Walk/anim.gltf", 
	//	"Mutant/Silly_Dancing/anim.gltf" 
	// ]

	CONSOLE_INFO("Loading animations for skeleton {}", skeleton.id);

	auto [it, success] = _skeletonAnimations.emplace(
		std::piecewise_construct,
		std::forward_as_tuple(skeleton.id),
		std::forward_as_tuple()
	);
	if (!success)
	{
		CONSOLE_ERROR("Error on loading animations");
		return nullptr;
	}

	Vector<Animation>& animVector = it->second;
	animVector.reserve(relativeAnims.size());
	for (const auto& relative : relativeAnims)
	{
		auto& animation = animVector.emplace_back(skeleton, relative);
		animation.id = animationId;

		_animationPaths.emplace_back(animation.id, relative);

		animationId++;
	}
	return &animVector;
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
	auto it = std::find_if(_animationPaths.begin(), _animationPaths.end(), [&](const std::pair<u32, fs::path>& pair) {
		return pair.first == animationID;
	});
	if(it == _animationPaths.end())
		return nullptr;

	return &it->second;
}

void AnimationsManager::CleanUp()
{
	for (auto& [skID, animations] : _skeletonAnimations)
		for (auto& animation : animations)
			animation.Destroy();
}

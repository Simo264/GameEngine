#include "AnimationsManager.hpp"

#include "Core/Log/Logger.hpp"
#include "Engine/Subsystems/ModelsManager.hpp"
#include "Engine/Filesystem/Filesystem.hpp"

const Vector<Animation>* AnimationsManager::LoadAnimations(const SkeletalMesh& skeleton,
																													 const Vector<fs::path> animationPaths)
{
	// GameEngine/Assets/Models/Skeletal/Mutant/Mutant.gltf
	const fs::path& absolute = skeleton.path;
	// Mutant/Mutant.gltf
	fs::path relative = fs::relative(absolute, Filesystem::GetSkeletalModelsPath());
	// Mutant/
	fs::path parent = relative.parent_path();
	CONSOLE_DEBUG("Loading animations for {}", parent.string());

	auto [it, success] = _animationsMap.emplace(
		std::piecewise_construct,
		std::forward_as_tuple(parent),
		std::forward_as_tuple()
	);
	if (!success)
	{
		CONSOLE_WARN("Error on loading animations");
		return nullptr;
	}
	
	if (animationPaths.empty())
	{
		CONSOLE_WARN("Current skeleton does not have animations");
		return &it->second;
	}

	// animationPaths = { 
	//	"Drunk_Walk/<filename>.gltf", 
	//	"Silly_Dancing/<filename>.gltf" 
	// }
	Vector<Animation>& animVector = it->second;
	animVector.reserve(animationPaths.size());
	for (const auto& p : animationPaths)
		animVector.emplace_back(skeleton, p);
	return &animVector;
}

const Vector<Animation>* AnimationsManager::GetAnimationsVector(const SkeletalMesh& skeleton)
{
	// GameEngine/Assets/Models/Skeletal/Mutant/Mutant.gltf
	const fs::path& absolute = skeleton.path; 
	// Mutant/Mutant.gltf
	fs::path relative = fs::relative(absolute, Filesystem::GetSkeletalModelsPath());
	// Mutant/
	fs::path parent = relative.parent_path();

	auto it = _animationsMap.find(parent);
	if (it != _animationsMap.end())
		return &it->second;
	
	return nullptr;
}

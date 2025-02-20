#include "Animator.hpp"

#include "Core/Log/Logger.hpp"

#include <stack>


// ----------------------------------------------------
//										PUBLIC													
// ----------------------------------------------------

void Animator::SetTargetSkeleton(SkeletalMesh& target)
{
	_targetSkeleton = &target;
	_boneTransforms.reserve(target.nrBones);
	for (u32 i = 0; i < target.nrBones; i++)
		_boneTransforms.push_back(mat4f(1.0f));
}
void Animator::SetTargetAnimation(const Animation* target)
{
	_targetAnimation = target;
	std::fill(_boneTransforms.begin(), _boneTransforms.end(), mat4f(1.0f));
}

void Animator::PlayAnimation()
{
	_playAnimation = true;
}
void Animator::PauseAnimation()
{
	_playAnimation = false;
}
void Animator::RestartAnimation()
{
	_currentTime = 0.0f;
}
void Animator::UpdateAnimation(f32 dt)
{
	if (!_playAnimation || !_targetSkeleton || !_targetAnimation)
		return;

	_currentTime += _targetAnimation->ticksPerSecond * dt;
	_currentTime = fmod(_currentTime, _targetAnimation->duration);
	
	CalculateBoneTransformRecursive(_targetSkeleton->rootNode, mat4f(1.0f));
}

// ---------------------------------------------------- 
//										PRIVATE														
// ---------------------------------------------------- 

void Animator::CalculateBoneTransformRecursive(const BoneNode& node, const mat4f& parentTransform)
{
#if 0
	std::stack<std::pair<const BoneNode*, mat4f>> stack;
	stack.push(std::make_pair(&node, parentTransform));

	while (!stack.empty())
	{
		auto& [currentNode, currentTransform] = stack.top();
		stack.pop();

		i32 boneIndex = currentNode->index;
		mat4f globalTransformation = currentTransform * currentNode->bindPoseTransform;
		if (boneIndex != -1)
		{
			// WARN: This piece of code collapses the performance
			Bone& bone = _targetSkeleton->bones[boneIndex];
			//InterpolateBone(bone, boneIndex);
			//globalTransformation = currentTransform * bone.localTransform;
			//const mat4f& offset = bone.offset;
			//_boneTransforms[boneIndex] = globalTransformation * offset;
		}

		for (const auto& child : currentNode->children)
		{
			stack.push(std::make_pair(&child, globalTransformation));
		}
	}
#endif

#if 1
	i32 boneIndex = node.index;
	mat4f globalTransformation = parentTransform * node.bindPoseTransform;
	if (boneIndex != -1)
	{
		// WARN: This piece of code collapses the performance
		Bone& bone = _targetSkeleton->bones[boneIndex];
		InterpolateBone(bone, boneIndex);
		globalTransformation = parentTransform * bone.localTransform;
		const mat4f& offset = bone.offset;
		_boneTransforms[boneIndex] = globalTransformation * offset;
	}

	for (auto& child : node.children)
		CalculateBoneTransformRecursive(child, globalTransformation);
#endif
}

void Animator::InterpolateBone(Bone& bone, u32 boneIndex)
{
	const auto& boneKeys = _targetAnimation->boneKeys;
	const auto& nrKeys = _targetAnimation->nrKeys;
	auto keys = std::find_if(boneKeys, boneKeys + nrKeys,
		[&](const AnimationKeys& k){
			return k.boneIndex == boneIndex;
	});

	mat4f translation = InterpolateBonePosition(*keys);
	mat4f rotation = InterpolateBoneRotation(*keys);
	mat4f scale = InterpolateBoneScale(*keys);
	bone.localTransform = translation * rotation * scale;
}
mat4f Animator::InterpolateBonePosition(const AnimationKeys& keys)
{
	if (keys.positionKeys.empty())
		return mat4f(1.0f);
	if (keys.positionKeys.size() == 1)
		return glm::translate(mat4f(1.0f), keys.positionKeys[0].position);

	const auto [currentKey, nextKey] = FindCurrentPositionKey(keys);
	f32 scaleFactor = CalculateBlendFactor(currentKey->timeStamp, nextKey->timeStamp);
	vec3f finalPosition = glm::mix(currentKey->position, nextKey->position, scaleFactor);
	return glm::translate(mat4f(1.0f), finalPosition);
}
mat4f Animator::InterpolateBoneRotation(const AnimationKeys& keys)
{
	if (keys.rotationKeys.empty())
		return mat4f(1.0f);
	if (keys.rotationKeys.size() == 1)
	{
		quat rotation = glm::normalize(keys.rotationKeys[0].orientation);
		return glm::mat4_cast(rotation);
	}

	const auto [currentKey, nextKey] = FindCurrentRotationKey(keys);
	f32 scaleFactor = CalculateBlendFactor(currentKey->timeStamp, nextKey->timeStamp);
	quat finalRotation = glm::slerp(currentKey->orientation, nextKey->orientation, scaleFactor);
	finalRotation = glm::normalize(finalRotation);
	return glm::mat4_cast(finalRotation);
}
mat4f Animator::InterpolateBoneScale(const AnimationKeys& keys)
{ 
	if (keys.scaleKeys.empty())
		return mat4f(1.0f);
	if (keys.scaleKeys.size() == 1)
		return glm::scale(mat4f(1.0f), keys.scaleKeys[0].scale);

	const auto [currentKey, nextKey] = FindCurrentScaleKey(keys);
	f32 scaleFactor = CalculateBlendFactor(currentKey->timeStamp, nextKey->timeStamp);
	vec3f finalScale = glm::mix(currentKey->scale, nextKey->scale, scaleFactor);
	return glm::scale(mat4f(1.0f), finalScale);
}

f32 Animator::CalculateBlendFactor(f32 prevTimestamp, f32 nextTimestamp) const
{
	f32 midWayLength = _currentTime - prevTimestamp;
	f32 framesDiff = nextTimestamp - prevTimestamp;
	return midWayLength / framesDiff;
}
std::pair<const KeyPosition*, const KeyPosition*> Animator::FindCurrentPositionKey(const AnimationKeys& keys) const
{
	for (u64 i = 0; i < keys.positionKeys.size() - 1; i++)
		if (_currentTime < keys.positionKeys[i + 1].timeStamp)
			return { &keys.positionKeys[i], &keys.positionKeys[i + 1] };
	return { nullptr, nullptr };
}
std::pair<const KeyRotation*, const KeyRotation*> Animator::FindCurrentRotationKey(const AnimationKeys& keys) const
{
	for (u64 i = 0; i < keys.rotationKeys.size() - 1; i++)
		if (_currentTime < keys.rotationKeys[i + 1].timeStamp)
			return { &keys.rotationKeys[i], &keys.rotationKeys[i + 1] };
	return { nullptr, nullptr };
}
std::pair<const KeyScale*, const KeyScale*>	Animator::FindCurrentScaleKey(const AnimationKeys& keys) const
{
	for (u64 i = 0; i < keys.scaleKeys.size() - 1; i++)
		if (_currentTime < keys.scaleKeys[i + 1].timeStamp)
			return { &keys.scaleKeys[i], &keys.scaleKeys[i + 1] };
	return { nullptr, nullptr };
}
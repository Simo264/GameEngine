#include "Animator.hpp"

#include "Core/Log/Logger.hpp"


// ----------------------------------------------------
//										PUBLIC													
// ----------------------------------------------------

void Animator::SetTargetSkeleton(SkeletalMesh& target)
{
	_targetSkeleton = &target;
	_boneTransforms.resize(target.bones.size(), mat4f(1.0f));
}
void Animator::SetTargetAnimation(const Animation* target)
{
	_targetAnimation = target;
	if (!target)
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

	_currentTime += _targetAnimation->TickPerSecond() * dt;
	_currentTime = fmod(_currentTime, _targetAnimation->Duration());
	CalculateBoneTransform(_targetSkeleton->rootNode, mat4f(1.0f));
}

// ---------------------------------------------------- 
//										PRIVATE														
// ---------------------------------------------------- 

void Animator::CalculateBoneTransform(const BoneNode& node, mat4f parentTransform)
{
	i32 boneIndex = node.boneIndex;
	mat4f nodeTransform = node.bindPoseTransform;
	mat4f globalTransformation = parentTransform * nodeTransform;
	
	if (boneIndex != -1)
	{
		Bone& bone = _targetSkeleton->bones.at(boneIndex);
		InterpolateBone(bone, boneIndex);
		nodeTransform = bone.localTransform;
		globalTransformation = parentTransform * nodeTransform;
		const mat4f& offset = bone.offset;
		_boneTransforms[boneIndex] = globalTransformation * offset;
	}

	for (auto& child : node.children)
		CalculateBoneTransform(child, globalTransformation);
}
void Animator::InterpolateBone(Bone& bone, u32 boneIndex)
{
	const auto& boneKeys = _targetAnimation->BoneKeys();
	auto keys = std::find_if(boneKeys.begin(), boneKeys.end(),
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
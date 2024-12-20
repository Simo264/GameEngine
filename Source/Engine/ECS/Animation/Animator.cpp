#include "Animator.hpp"

/* ---------------------------------------------------- */
/*										PUBLIC														*/
/* ---------------------------------------------------- */

void Animator::UpdateAnimation(f32 dt)
{
	if (!_animationTarget || !_skeletonTarget)
		return;

	_currentTime += _animationTarget->ticksPerSecond * dt;
	_currentTime = fmod(_currentTime, _animationTarget->duration);
	CalculateBoneTransform(_skeletonTarget->rootNode, mat4f(1.0f));
}

void Animator::PlayAnimation(Animation& animation)
{
	_animationTarget = &animation;
	_currentTime = 0.0f;
}

void Animator::CalculateBoneTransform(const BoneNode& node, mat4f parentTransform)
{
	const String& nodeName = node.name;
	mat4f nodeTransform = node.transformation;
	mat4f globalTransformation = parentTransform * nodeTransform;

	auto [bone, boneIndex] = _skeletonTarget->FindBone(nodeName);
	if (bone)
	{
		InterpolateBone(bone, nodeName); //Bone->Interpolate(_currentTime);
		nodeTransform = bone->localTransform;
		globalTransformation = parentTransform * nodeTransform;
		const mat4f& offset = bone->offset;
		_boneTransforms[boneIndex] = globalTransformation * offset;
	}

	for(auto& child : node.children)
		CalculateBoneTransform(child, globalTransformation);
}

/* ---------------------------------------------------- */
/*										PRIVATE														*/
/* ---------------------------------------------------- */

void Animator::InterpolateBone(Bone* bone, StringView boneName)
{
	AnimationKeys& keys = _animationTarget->boneKeys.at(boneName.data());
	mat4f translation = InterpolateBonePosition(keys);
	mat4f rotation = InterpolateBoneRotation(keys);
	mat4f scale = InterpolateBoneScale(keys);
	bone->localTransform = translation * rotation * scale;
}

mat4f Animator::InterpolateBonePosition(AnimationKeys& keys)
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
mat4f Animator::InterpolateBoneRotation(AnimationKeys& keys)
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
mat4f Animator::InterpolateBoneScale(AnimationKeys& keys)
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

f32 Animator::CalculateBlendFactor(f32 prevTimestamp, f32 nextTimestamp)
{
	f32 midWayLength = _currentTime - prevTimestamp;
	f32 framesDiff = nextTimestamp - prevTimestamp;
	return midWayLength / framesDiff;
}
std::pair<KeyPosition*, KeyPosition*> Animator::FindCurrentPositionKey(AnimationKeys& keys)
{
	for (u64 i = 0; i < keys.positionKeys.size() - 1; i++)
		if (_currentTime < keys.positionKeys[i + 1].timeStamp)
			return { &keys.positionKeys[i], &keys.positionKeys[i + 1] };
	return { nullptr, nullptr };
}
std::pair<KeyRotation*, KeyRotation*> Animator::FindCurrentRotationKey(AnimationKeys& keys)
{
	for (u64 i = 0; i < keys.rotationKeys.size() - 1; i++)
		if (_currentTime < keys.rotationKeys[i + 1].timeStamp)
			return { &keys.rotationKeys[i], &keys.rotationKeys[i + 1] };
	return { nullptr, nullptr };
}
std::pair<KeyScale*, KeyScale*>	Animator::FindCurrentScaleKey(AnimationKeys& keys)
{
	for (u64 i = 0; i < keys.scaleKeys.size() - 1; i++)
		if (_currentTime < keys.scaleKeys[i + 1].timeStamp)
			return { &keys.scaleKeys[i], &keys.scaleKeys[i + 1] };
	return { nullptr, nullptr };
}
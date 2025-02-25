#include "Animator.hpp"

#include "Core/Log/Logger.hpp"

#include <stack>


// ----------------------------------------------------
//										PUBLIC													
// ----------------------------------------------------

Animator::Animator() :
	boneTransforms{},
	nrBoneTransforms{ 0 },
	currentTime{ 0.f },
	_targetSkeleton{ nullptr },
	_targetAnimation{ nullptr },
	_playAnimation{ false }
{
}


void Animator::SetTargetSkeleton(SkeletalMesh& target)
{
	if (_targetSkeleton)
		return;
	
	_targetSkeleton = &target;
	boneTransforms = std::make_unique<mat4f[]>(target.nrBones);
	nrBoneTransforms = target.nrBones;
	for (u32 i = 0; i < nrBoneTransforms; i++)
		boneTransforms[i] = mat4f(1.0f);
}
void Animator::SetTargetAnimation(const Animation* target)
{
	_targetAnimation = target;
	for (u32 i = 0; i < nrBoneTransforms; i++)
		boneTransforms[i] = mat4f(1.0f);
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
	currentTime = 0.0f;
}
void Animator::UpdateAnimation(f32 dt)
{
	if (!_playAnimation || !_targetSkeleton || !_targetAnimation)
		return;

	currentTime += _targetAnimation->ticksPerSecond * dt;
	currentTime = fmod(currentTime, _targetAnimation->duration);
	UpdateBoneTransform(*_targetSkeleton->rootNode, mat4f(1.0f));
}

// ---------------------------------------------------- 
//										PRIVATE														
// ---------------------------------------------------- 

void Animator::UpdateBoneTransform(const BoneNode& node, const mat4f& parentTransform)
{
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
			Bone& bone = _targetSkeleton->bones[boneIndex];
			InterpolateBone(boneIndex);
			globalTransformation = currentTransform * bone.localTransform;
			const mat4f& offset = bone.offset;
			boneTransforms[boneIndex] = globalTransformation * offset;
		}

		for (const auto& child : currentNode->children)
		{
			stack.push(std::make_pair(&child, globalTransformation));
		}
	}
}

void Animator::InterpolateBone(u32 boneIndex)
{
	const auto& boneKeys = _targetAnimation->bonesAnimKeys[boneIndex];
	mat4f translation = InterpolateBonePosition(boneKeys);
	mat4f rotation = InterpolateBoneRotation(boneKeys);
	mat4f scale = InterpolateBoneScale(boneKeys);
	Bone& bone = _targetSkeleton->bones[boneIndex];
	bone.localTransform = translation * rotation * scale;
}
mat4f Animator::InterpolateBonePosition(const BoneAnimationKeys& boneKeys)
{
	if (boneKeys.nrPosKeys == 0)
		return mat4f(1.0f);
	if (boneKeys.nrPosKeys == 1)
		return glm::translate(mat4f(1.0f), boneKeys.posKeys[0].position);

	const auto [currentKey, nextKey] = FindCurrentPositionKey(boneKeys);
	f32 scaleFactor = CalculateBlendFactor(currentKey->timeStamp, nextKey->timeStamp);
	vec3f finalPosition = glm::mix(currentKey->position, nextKey->position, scaleFactor);
	return glm::translate(mat4f(1.0f), finalPosition);
}
mat4f Animator::InterpolateBoneRotation(const BoneAnimationKeys& boneKeys)
{
	if (boneKeys.nrRotKeys == 0)
		return mat4f(1.0f);
	if (boneKeys.nrRotKeys == 1)
		return glm::mat4_cast(glm::normalize(boneKeys.rotKeys[0].orientation));

	const auto [currentKey, nextKey] = FindCurrentRotationKey(boneKeys);
	f32 scaleFactor = CalculateBlendFactor(currentKey->timeStamp, nextKey->timeStamp);
	quat finalRotation = glm::slerp(currentKey->orientation, nextKey->orientation, scaleFactor);
	finalRotation = glm::normalize(finalRotation);
	return glm::mat4_cast(finalRotation);
}
mat4f Animator::InterpolateBoneScale(const BoneAnimationKeys& boneKeys)
{ 
	if (boneKeys.nrScaleKeys == 0)
		return mat4f(1.0f);
	if (boneKeys.nrScaleKeys == 1)
		return glm::scale(mat4f(1.0f), boneKeys.scaleKeys[0].scale);

	const auto [currentKey, nextKey] = FindCurrentScaleKey(boneKeys);
	f32 scaleFactor = CalculateBlendFactor(currentKey->timeStamp, nextKey->timeStamp);
	vec3f finalScale = glm::mix(currentKey->scale, nextKey->scale, scaleFactor);
	return glm::scale(mat4f(1.0f), finalScale);
}

f32 Animator::CalculateBlendFactor(f32 prevTimestamp, f32 nextTimestamp) const
{
	f32 midWayLength = currentTime - prevTimestamp;
	f32 framesDiff = nextTimestamp - prevTimestamp;
	return midWayLength / framesDiff;
}
std::pair<const KeyPosition*, const KeyPosition*> Animator::FindCurrentPositionKey(const BoneAnimationKeys& boneKeys) const
{
	for (u32 i = 0; i < boneKeys.nrPosKeys - 1; i++)
		if (currentTime < boneKeys.posKeys[i + 1].timeStamp)
			return { &boneKeys.posKeys[i], &boneKeys.posKeys[i + 1] };
	return { nullptr, nullptr };
}
std::pair<const KeyRotation*, const KeyRotation*> Animator::FindCurrentRotationKey(const BoneAnimationKeys& boneKeys) const
{
	for (u32 i = 0; i < boneKeys.nrRotKeys - 1; i++)
		if (currentTime < boneKeys.rotKeys[i + 1].timeStamp)
			return { &boneKeys.rotKeys[i], &boneKeys.rotKeys[i + 1] };
	return { nullptr, nullptr };
}
std::pair<const KeyScale*, const KeyScale*>	Animator::FindCurrentScaleKey(const BoneAnimationKeys& boneKeys) const
{
	for (u32 i = 0; i < boneKeys.nrScaleKeys - 1; i++)
		if (currentTime < boneKeys.scaleKeys[i + 1].timeStamp)
			return { &boneKeys.scaleKeys[i], &boneKeys.scaleKeys[i + 1] };
	return { nullptr, nullptr };
}
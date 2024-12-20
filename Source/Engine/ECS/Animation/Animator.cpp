#include "Animator.hpp"


/* ---------------------------------------------------- */
/*										PUBLIC														*/
/* ---------------------------------------------------- */

void Animator::UpdateAnimation(f32 dt)
{
	if (_currentAnimation)
	{
		_currentTime += _currentAnimation->ticksPerSecond * dt;
		_currentTime = fmod(_currentTime, _currentAnimation->duration);
		CalculateBoneTransform(_currentAnimation->rootNode, mat4f(1.0f));
	}
}

void Animator::PlayAnimation(Animation* pAnimation)
{
	_currentAnimation = pAnimation;
	_currentTime = 0.0f;
}

void Animator::CalculateBoneTransform(const BoneNode& node, mat4f parentTransform)
{
	const String& nodeName = node.name;
	mat4f nodeTransform = node.transformation;

	Bone* Bone = _currentAnimation->FindBone(nodeName);
	if (Bone)
	{
		Bone->Interpolate(_currentTime);
		nodeTransform = Bone->LocalTransform();
	}

	mat4f globalTransformation = parentTransform * nodeTransform;

	const auto* boneInfoMap = _currentAnimation->boneMap;
	auto it = boneInfoMap->find(nodeName);
	if (it != boneInfoMap->end())
	{
		i32 index = it->second.id;
		const mat4f& offset = it->second.offset;
		_boneTransforms[index] = globalTransformation * offset;
	}

	for(auto& child : node.children)
		CalculateBoneTransform(child, globalTransformation);
}

/* ---------------------------------------------------- */
/*										PRIVATE														*/
/* ---------------------------------------------------- */

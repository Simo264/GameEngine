#include "Animator.hpp"


/* ---------------------------------------------------- */
/*										PUBLIC														*/
/* ---------------------------------------------------- */

void Animator::UpdateAnimation(f32 dt)
{
	if (_currentAnimation)
	{
		_currentTime += _currentAnimation->GetTicksPerSecond() * dt;
		_currentTime = fmod(_currentTime, _currentAnimation->GetDuration());
		CalculateBoneTransform(&_currentAnimation->GetRootNode(), mat4f(1.0f));
	}
}

void Animator::PlayAnimation(Animation* pAnimation)
{
	_currentAnimation = pAnimation;
	_currentTime = 0.0f;
}

void Animator::CalculateBoneTransform(const AssimpNodeData* node, mat4f parentTransform)
{
	const String& nodeName = node->name;
	mat4f nodeTransform = node->transformation;
	Bone* Bone = _currentAnimation->FindBone(nodeName);
	if (Bone)
	{
		Bone->Update(_currentTime);
		nodeTransform = Bone->GetLocalTransform();
	}

	mat4f globalTransformation = parentTransform * nodeTransform;

	const auto* boneInfoMap = _currentAnimation->GetBoneInfoMap();
	auto it = boneInfoMap->find(nodeName);
	if (it != boneInfoMap->end())
	{
		i32 index = it->second.id;
		const mat4f& offset = it->second.offset;
		_finalBoneMatrices.at(index) = globalTransformation * offset;
	}

	for (u32 i = 0; i < node->childrenCount; i++)
		CalculateBoneTransform(&node->children[i], globalTransformation);
}

/* ---------------------------------------------------- */
/*										PRIVATE														*/
/* ---------------------------------------------------- */

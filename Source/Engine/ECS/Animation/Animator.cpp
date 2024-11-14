#include "Animator.hpp"


/* ---------------------------------------------------- */
/*										PUBLIC														*/
/* ---------------------------------------------------- */

Animator::Animator(Animation* animation)
{
	m_CurrentTime = 0.0;
	m_CurrentAnimation = animation;

	m_FinalBoneMatrices.reserve(100);
	for (i32 i = 0; i < 100; i++)
		m_FinalBoneMatrices.push_back(mat4f(1.0f));
}

void Animator::UpdateAnimation(f32 dt)
{
	m_DeltaTime = dt;
	if (m_CurrentAnimation)
	{
		m_CurrentTime += m_CurrentAnimation->GetTicksPerSecond() * dt;
		m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->GetDuration());
		CalculateBoneTransform(&m_CurrentAnimation->GetRootNode(), glm::mat4(1.0f));
	}
}

void Animator::PlayAnimation(Animation* pAnimation)
{
	m_CurrentAnimation = pAnimation;
	m_CurrentTime = 0.0f;
}

void Animator::CalculateBoneTransform(const AssimpNodeData* node, mat4f parentTransform)
{
	const String& nodeName = node->name;
	mat4f nodeTransform = node->transformation;

	Bone* Bone = m_CurrentAnimation->FindBone(nodeName);
	if (Bone)
	{
		Bone->Update(m_CurrentTime);
		nodeTransform = Bone->GetLocalTransform();
	}

	mat4f globalTransformation = parentTransform * nodeTransform;

	auto& boneInfoMap = m_CurrentAnimation->GetBoneIDMap();
	if (boneInfoMap.find(nodeName) != boneInfoMap.end())
	{
		i32 index = boneInfoMap.at(nodeName).id;
		mat4f offset = boneInfoMap.at(nodeName).offset;
		m_FinalBoneMatrices[index] = globalTransformation * offset;
	}

	for (i32 i = 0; i < node->childrenCount; i++)
		CalculateBoneTransform(&node->children[i], globalTransformation);
}

/* ---------------------------------------------------- */
/*										PRIVATE														*/
/* ---------------------------------------------------- */

#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Core.hpp"
#include "Animation.hpp"

class Animator
{
public:
	Animator(Animation* animation);
	~Animator() = default;

	void UpdateAnimation(f32 dt);
	void PlayAnimation(Animation* pAnimation);
	void CalculateBoneTransform(const AssimpNodeData* node, mat4f parentTransform);
	const Vector<mat4f>& GetFinalBoneMatrices() const { return m_FinalBoneMatrices; }

private:
	Vector<mat4f> m_FinalBoneMatrices;
	Animation* m_CurrentAnimation;
	f32 m_CurrentTime;
	f32 m_DeltaTime;
};

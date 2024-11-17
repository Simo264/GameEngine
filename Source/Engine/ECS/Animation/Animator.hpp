#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Core.hpp"
#include "Animation.hpp"

class Animator
{
public:
	Animator(Animation* animation) :
		_currentAnimation{ animation },
		_currentTime{ 0.f },
		_finalBoneMatrices(100, mat4f(1.0f))
	{}
	~Animator() = default;

	void UpdateAnimation(f32 dt);
	void PlayAnimation(Animation* pAnimation);
	void CalculateBoneTransform(const AssimpNodeData* node, mat4f parentTransform);
	const Vector<mat4f>& GetFinalBoneMatrices() const { return _finalBoneMatrices; }

private:
	Vector<mat4f> _finalBoneMatrices;
	Animation* _currentAnimation;
	f32 _currentTime;
};

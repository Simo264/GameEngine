#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Core.hpp"
#include "Animation.hpp"

class Animator
{
public:
	Animator(Animation* animation)
		: _currentTime{ 0 },
			_currentAnimation{ animation },
			_deltaTime{ 0 },
			_finalBoneMatrices(100, mat4f(1.0f))
	{}
	~Animator() = default;

	void UpdateAnimation(f32 delta);
	void PlayAnimation(Animation* animation);
	void CalculateBoneTransform(const AssimpNodeData* node, const mat4f& parentTransform);

	Vector<mat4f>& GetFinalBoneMatrices() { return _finalBoneMatrices; }

private:
	Vector<mat4f> _finalBoneMatrices;
	Animation* _currentAnimation;
	f32 _currentTime;
	f32 _deltaTime;
};

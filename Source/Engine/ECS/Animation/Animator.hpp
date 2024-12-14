#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Core.hpp"
#include "Engine/ECS/Animation/Animation.hpp"

class Animator
{
public:
	Animator(Animation& animation) :
		_currentAnimation{ &animation },
		_currentTime{ 0.f },
		_boneTransforms(100, mat4f(1.0f))
	{}
	~Animator() = default;

	void UpdateAnimation(f32 dt);
	void PlayAnimation(Animation* pAnimation);
	void CalculateBoneTransform(const BoneNode& node, mat4f parentTransform);
	const Vector<mat4f>& GetBoneTransforms() const { return _boneTransforms; }

private:
	Vector<mat4f> _boneTransforms;
	Animation* _currentAnimation;
	f32 _currentTime;
};

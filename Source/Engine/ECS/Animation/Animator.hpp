#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Core.hpp"
#include "Engine/ECS/Animation/Animation.hpp"

class Animator
{
public:
	Animator(SkeletonMesh& skeleton, Animation& animation) :
		_animationTarget{ &animation },
		_skeletonTarget{ &skeleton },
		_currentTime{ 0.f },
		_boneTransforms(100, mat4f(1.0f))
	{}
	~Animator() = default;

	void UpdateAnimation(f32 dt);
	void PlayAnimation(Animation& pAnimation);
	void CalculateBoneTransform(const BoneNode& node, mat4f parentTransform);
	const Vector<mat4f>& GetBoneTransforms() const { return _boneTransforms; }

private:
	void InterpolateBone(Bone* bone, StringView boneName);
	mat4f InterpolateBonePosition(AnimationKeys& keys);
	mat4f InterpolateBoneRotation(AnimationKeys& keys);
	mat4f InterpolateBoneScale(AnimationKeys& keys);

	f32 CalculateBlendFactor(f32 prevTimestamp, f32 nextTimestamp);
	std::pair<KeyPosition*, KeyPosition*> FindCurrentPositionKey(AnimationKeys& keys);
	std::pair<KeyRotation*, KeyRotation*> FindCurrentRotationKey(AnimationKeys& keys);
	std::pair<KeyScale*, KeyScale*>				FindCurrentScaleKey(AnimationKeys& keys);
	
	Vector<mat4f> _boneTransforms;
	f32 _currentTime;
	
	Animation*		_animationTarget;
	SkeletonMesh* _skeletonTarget;
};

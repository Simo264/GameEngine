#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Base.hpp"
#include "Engine/ECS/Skeleton/SkeletalMesh.hpp"
#include "Engine/ECS/Animation/Animation.hpp"

/**
 * @class Animator
 * @brief Manages and updates animations for a skeleton mesh.
 */
class Animator
{
public:
	Animator() :
		animationsRef{ nullptr },
		_playAnimation{ false },
		_currentTime{ 0.f },
		_targetSkeleton{ nullptr },
		_targetAnimation{ nullptr },
		_boneTransforms{}
	{}

	/** @brief Default destructor for the Animator class. */
	~Animator() = default;

	void SetTargetSkeleton(SkeletalMesh& target);
	void SetTargetAnimation(const Animation* target);

	void UpdateAnimation(f32 dt);
	void PlayAnimation();
	void PauseAnimation();
	void RestartAnimation();

	f32 CurrentTime() const { return _currentTime; }
	const Animation* GetAttachedAnimation() const { return _targetAnimation; }
	const auto& GetBoneTransforms() const { return _boneTransforms; }

	// Reference to the animations vector in AnimationsManager
	const Vector<Animation>* animationsRef; 

private:
	void CalculateBoneTransform(const BoneNode& node, mat4f parentTransform);
	void InterpolateBone(Bone& bone, u32 boneIndex);
	mat4f InterpolateBonePosition(const AnimationKeys& keys);
	mat4f InterpolateBoneRotation(const AnimationKeys& keys);
	mat4f InterpolateBoneScale(const AnimationKeys& keys);

	f32 CalculateBlendFactor(f32 prevTimestamp, f32 nextTimestamp) const;
	std::pair<const KeyPosition*, const KeyPosition*> FindCurrentPositionKey(const AnimationKeys& keys) const;
	std::pair<const KeyRotation*, const KeyRotation*> FindCurrentRotationKey(const AnimationKeys& keys) const;
	std::pair<const KeyScale*,		const KeyScale*>		FindCurrentScaleKey(const AnimationKeys& keys) const;
	
	bool _playAnimation;
	f32 _currentTime;
	Vector<mat4f> _boneTransforms;
	
	SkeletalMesh* _targetSkeleton;
	const Animation* _targetAnimation;
};

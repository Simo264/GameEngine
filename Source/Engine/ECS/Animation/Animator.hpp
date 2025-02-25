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
	Animator();
	~Animator() = default;

	/** @brief Move constructor */
	Animator(Animator&&) noexcept = default;
	Animator& operator=(Animator&&) noexcept = default;

	/** @brief Delete copy constructor */
	Animator(const Animator&) = delete;
	Animator& operator=(const Animator&) = delete;

	void SetTargetSkeleton(SkeletalMesh& target);
	void SetTargetAnimation(const Animation* target);

	const Animation* GetAttachedAnimation() const { return _targetAnimation; }

	void UpdateAnimation(f32 dt);
	void PlayAnimation();
	void PauseAnimation();
	void RestartAnimation();

	UniquePtr<mat4f[]> boneTransforms;
	u32 nrBoneTransforms;
	f32 currentTime;
	
private:
	void UpdateBoneTransform(const BoneNode& node, const mat4f& parentTransform);

	void InterpolateBone(u32 boneIndex);
	mat4f InterpolateBonePosition(const BoneAnimationKeys& boneKeys);
	mat4f InterpolateBoneRotation(const BoneAnimationKeys& boneKeys);
	mat4f InterpolateBoneScale(const BoneAnimationKeys& boneKeys);

	f32 CalculateBlendFactor(f32 prevTimestamp, f32 nextTimestamp) const;
	std::pair<const KeyPosition*, const KeyPosition*> FindCurrentPositionKey(const BoneAnimationKeys& boneKeys) const;
	std::pair<const KeyRotation*, const KeyRotation*> FindCurrentRotationKey(const BoneAnimationKeys& boneKeys) const;
	std::pair<const KeyScale*,		const KeyScale*>		FindCurrentScaleKey(const BoneAnimationKeys& boneKeys) const;

	SkeletalMesh* _targetSkeleton;
	const Animation* _targetAnimation;
	bool _playAnimation;
};

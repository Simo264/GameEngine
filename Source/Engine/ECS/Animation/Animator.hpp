#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Core.hpp"
#include "Engine/ECS/Animation/Animation.hpp"

/**
 * @class Animator
 * @brief Manages and updates animations for a skeleton mesh.
 */
class Animator
{
public:
	Animator(SkeletonMesh& skeleton) :
		_playAnimation{ false },
		_currentTime{ 0.f },
		_boneTransforms(100, mat4f(1.0f)),
		_skeletonAttached{ &skeleton },
		_animationAttached{ nullptr, nullptr }
	{}

	/**
	 * @brief Default destructor for the Animator class.
	 */
	~Animator() = default;

	std::pair<const char*, const Animation*> InsertAnimation(const fs::path& animationPath);

	void AttachAnimation(StringView animationName);
	
	void DetachAnimation();

	void UpdateAnimation(f32 dt);
	
	void PlayAnimation();

	void PauseAnimation();

	void RestartAnimation();

	std::pair<const char*, const Animation*> FindAnimation(StringView animationName);
	std::pair<const char*, const Animation*> AttachedAnimation() const { return _animationAttached; }
	f32 CurrentTime() const { return _currentTime; }
	const Map<String, Animation>& AnimationsMap() const { return _animationsMap; }
	const Vector<mat4f>& BoneTransforms() const { return _boneTransforms; }
	const SkeletonMesh& AttachedSkeleton() const { return *_skeletonAttached; }
private:
	void CalculateBoneTransform(const BoneNode& node, mat4f parentTransform);
	void InterpolateBone(Bone* bone, u32 boneIndex);
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
	Map<String, Animation> _animationsMap;
	
	SkeletonMesh* _skeletonAttached;
	std::pair<const char*, const Animation*> _animationAttached;
};

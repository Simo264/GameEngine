#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Base.hpp"
#include "Engine/ECS/Skeleton/SkeletalMesh.hpp"

struct aiAnimation;
struct aiNodeAnim;
struct aiNode;

struct KeyPosition
{
	f32 timeStamp{};
	vec3f position{};
};
struct KeyRotation
{
	f32 timeStamp{};
	quat orientation{};
};
struct KeyScale
{
	f32 timeStamp{};
	vec3f scale{};
};
struct AnimationKeys
{
	i32 boneIndex{};
	Vector<KeyPosition> positionKeys{};
	Vector<KeyRotation> rotationKeys{};
	Vector<KeyScale>		scaleKeys{};
};

/**
 * @class Animation
 * @brief Represents an animation associated with a skeleton mesh.
 */
class Animation
{
public:
	/**
	 * @brief Constructs an Animation object by loading animation data from the specified file path.
	 * @param path The file path to the animation data.
	 * @param skeleton The skeleton mesh associated with the animation.
	 */
	Animation(const fs::path& path, const SkeletalMesh& skeleton);
	
	/** @brief Default destructor for the Animation class. */
	~Animation() = default;

	f32 Duration() const { return _duration; }
	f32 TickPerSecond() const { return _ticksPerSecond; }
	const Vector<AnimationKeys>& BoneKeys() const { return _boneKeys; }
	const fs::path& Path() const { return _path; }
private:
	void LoadAnimation(const aiAnimation* animation, const SkeletalMesh& skeleton);
	void LoadBoneKeys(AnimationKeys& keys, const aiNodeAnim* channel);

	fs::path _path;
	f32 _duration;
	f32 _ticksPerSecond;
	Vector<AnimationKeys> _boneKeys;
};
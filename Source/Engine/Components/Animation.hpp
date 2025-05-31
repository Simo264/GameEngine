#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Base.hpp"

class SkeletalMesh;

struct aiAnimation;
struct aiNodeAnim;
struct aiNode;

struct KeyPosition
{
	f32 timeStamp{};
	Vec3F position{};
};
struct KeyRotation
{
	f32 timeStamp{};
	Quaternion orientation{};
};
struct KeyScale
{
	f32 timeStamp{};
	Vec3F scale{};
};
struct BoneAnimationKeys
{
	UniquePointer<KeyPosition[]> posKeys;
	u32 nrPosKeys;

	UniquePointer<KeyRotation[]> rotKeys;
	u32 nrRotKeys;

	UniquePointer<KeyScale[]> scaleKeys;
	u32 nrScaleKeys;
};


/** @brief Represents an animation associated with a skeleton mesh. */
class Animation
{
public:
	/**
	 * @brief Constructs an animation for a given skeletal mesh.
	 *
	 * This constructor initializes an animation by associating it with a specific skeletal mesh
	 * and loading the animation data from the given relative path.
	 *
	 * @param skeleton The skeletal mesh to which this animation belongs.
	 * 
	 * @param relative The relative path to the animation file inside the skeletal model directory.
	 * Example: "Mutant/Drunk_Walk/anim.gltf".
	 */
	Animation(const SkeletalMesh& skeleton, const fs::path& relative);
	~Animation() = default;

	/** @brief Move constructor */
	Animation(Animation&&) noexcept = default;
	Animation& operator=(Animation&&) noexcept = default;

	/** @brief Delete copy constructor */
	Animation(const Animation&) = delete;
	Animation& operator=(const Animation&) = delete;
	
	UniquePointer<BoneAnimationKeys[]> bonesAnimKeys;
	u32 nrKeys;

	f32 duration;
	f32 ticksPerSecond;
	u32 id;
	
private:
	void LoadBoneKeys(BoneAnimationKeys& boneKeys, const aiNodeAnim* channel);
};
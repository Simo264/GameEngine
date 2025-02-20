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
	
	void Destroy();

	AnimationKeys* boneKeys;
	u32 nrKeys;

	f32 duration;
	f32 ticksPerSecond;
	u32 id;
	
private:
	void LoadAnimation(const aiAnimation* animation, const SkeletalMesh& skeleton);
	void LoadBoneKeys(AnimationKeys& keys, const aiNodeAnim* channel);
};
#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Core.hpp"

struct aiNodeAnim;

struct BoneInfo
{
	i32 id{};
	mat4f offset{};
};

struct BoneNode
{
	mat4f transformation{};
	String name{};
	Vector<BoneNode> children{};
};

struct KeyPosition
{
	vec3f position{};
	f32 timeStamp{};
};

struct KeyRotation
{
	quat orientation{};
	f32 timeStamp{};
};

struct KeyScale
{
	vec3f scale{};
	f32 timeStamp{};
};

class Bone
{
public:
	Bone(StringView name, i32 id, const aiNodeAnim* channel);

	void Interpolate(f32 animationTime);

	const mat4f& GetLocalTransform() const { return _localTransform; }
	const String& GetBoneName() const { return _name; }
	i32 GetBoneID() const { return _id; }

	i32 GetPositionIndex(f32 animationTime);
	i32 GetRotationIndex(f32 animationTime);
	i32 GetScaleIndex(f32 animationTime);

private:
	Vector<KeyPosition> _positions;
	Vector<KeyRotation> _rotations;
	Vector<KeyScale>		_scales;

	i32			_id;
	String	_name;
	mat4f		_localTransform;

	f32 GetScaleFactor(f32 lastTimeStamp, f32 nextTimeStamp, f32 animationTime);
	mat4f InterpolatePosition(f32 animationTime);
	mat4f InterpolateRotation(f32 animationTime);
	mat4f InterpolateScaling(f32 animationTime);
};

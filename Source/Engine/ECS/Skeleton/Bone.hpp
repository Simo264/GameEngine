#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Core.hpp"

struct aiNodeAnim;

struct BoneInfo
{
	u32 id{};
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

class Bone
{
public:
	Bone(StringView name, u32 id, const aiNodeAnim* channel);
	~Bone();

	void Interpolate(f32 currentTime);
	mat4f LocalTransform() const { return _localTransform; }

	u32	id;
	String name;
	
private:
	mat4f InterpolatePosition(f32 currentTime);
	mat4f InterpolateRotation(f32 currentTime);
	mat4f InterpolateScaling(f32 currentTime);
	f32 CalculateBlendFactor(f32 prevTimestamp, f32 nextTimestamp, f32 currentTime);
	std::pair<KeyPosition*, KeyPosition*> FindCurrentPositionKey(f32 currentTime);
	std::pair<KeyRotation*, KeyRotation*> FindCurrentRotationKey(f32 currentTime);
	std::pair<KeyScale*, KeyScale*>				FindCurrentScaleKey(f32 currentTime);

	mat4f	_localTransform;
	Vector<KeyPosition> _positionKeys;
	Vector<KeyRotation> _rotationKeys;
	Vector<KeyScale>		_scaleKeys;
};

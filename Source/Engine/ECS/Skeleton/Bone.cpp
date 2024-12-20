#include "Bone.hpp"

#include "Core/Math/Ext.hpp"
#include "Core/Log/Logger.hpp"

#include <assimp/scene.h>




static mat4f AiMatrixToGLM(const aiMatrix4x4& from)
{
	mat4f to{};
	//the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
	to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
	to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
	to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
	to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
	return to;
}


/* ---------------------------------------------------- */
/*										PUBLIC														*/
/* ---------------------------------------------------- */

Bone::Bone(StringView name, u32 id, const aiNodeAnim* channel) :
	name{ name.data() },
	id{ id },
	_localTransform{},
	_positionKeys{},
	_rotationKeys{},
	_scaleKeys{}
{
	_positionKeys.reserve(channel->mNumPositionKeys);
	_rotationKeys.reserve(channel->mNumRotationKeys);
	_scaleKeys.reserve(channel->mNumScalingKeys);

	for (u32 i = 0; i < channel->mNumPositionKeys; i++)
	{
		f32 timestamp = channel->mPositionKeys[i].mTime;
		vec3f position = vec3f(
			channel->mPositionKeys[i].mValue.x,
			channel->mPositionKeys[i].mValue.y,
			channel->mPositionKeys[i].mValue.z
		);
		auto& key = _positionKeys.emplace_back();
		key.timeStamp = timestamp;
		key.position = position;
	}
	for (u32 i = 0; i < channel->mNumRotationKeys; i++)
	{
		f32 timestamp = channel->mRotationKeys[i].mTime;
		quat orientation = quat(
			channel->mRotationKeys[i].mValue.w,
			channel->mRotationKeys[i].mValue.x,
			channel->mRotationKeys[i].mValue.y,
			channel->mRotationKeys[i].mValue.z
		);
		auto& key = _rotationKeys.emplace_back();
		key.timeStamp = timestamp;
		key.orientation = orientation;
	}
	for (u32 i = 0; i < channel->mNumScalingKeys; i++)
	{
		f32 timestamp = channel->mScalingKeys[i].mTime;
		vec3f scale = vec3f(
			channel->mScalingKeys[i].mValue.x,
			channel->mScalingKeys[i].mValue.y,
			channel->mScalingKeys[i].mValue.z
		);
		auto& key = _scaleKeys.emplace_back();
		key.timeStamp = timestamp;
		key.scale = scale;
	}
}
Bone::~Bone()
{
	CONSOLE_DEBUG("Destroy bone {}", name);

	//if (_positionKeys)
	//	delete[] _positionKeys;
	//if (_rotationKeys )
	//	delete[] _rotationKeys;
	//if (_scaleKeys)
	//	delete[] _scaleKeys;
}

void Bone::Interpolate(f32 currentTime)
{
	mat4f translation = InterpolatePosition(currentTime);
	mat4f rotation		= InterpolateRotation(currentTime);
	mat4f scale				= InterpolateScaling(currentTime);
	_localTransform		= translation * rotation * scale;
}

/* ---------------------------------------------------- */
/*										PRIVATE														*/
/* ---------------------------------------------------- */

f32 Bone::CalculateBlendFactor(f32 prevTimestamp, f32 nextTimestamp, f32 currentTime)
{ 
	f32 midWayLength = currentTime - prevTimestamp;
	f32 framesDiff = nextTimestamp - prevTimestamp;
	return midWayLength / framesDiff;
}
mat4f Bone::InterpolatePosition(f32 currentTime)
{
	if (_positionKeys.empty())
		return mat4f(1.0f);
	if(_positionKeys.size() == 1)
		return glm::translate(mat4f(1.0f), _positionKeys[0].position);
	
	const auto [currentKey, nextKey] = FindCurrentPositionKey(currentTime);
	f32 scaleFactor = CalculateBlendFactor(currentKey->timeStamp, nextKey->timeStamp, currentTime);
	vec3f finalPosition = glm::mix(currentKey->position, nextKey->position, scaleFactor);
	return glm::translate(mat4f(1.0f), finalPosition);
}
mat4f Bone::InterpolateRotation(f32 currentTime)
{
	if (_rotationKeys.empty())
		return mat4f(1.0f);
	if (_rotationKeys.size() == 1)
	{
		quat rotation = glm::normalize(_rotationKeys[0].orientation);
		return glm::mat4_cast(rotation);
	}

	const auto [currentKey, nextKey] = FindCurrentRotationKey(currentTime);
	f32 scaleFactor = CalculateBlendFactor(currentKey->timeStamp, nextKey->timeStamp, currentTime);
	quat finalRotation = glm::slerp(currentKey->orientation, nextKey->orientation, scaleFactor);
	finalRotation = glm::normalize(finalRotation);
	return glm::mat4_cast(finalRotation);

}
mat4f Bone::InterpolateScaling(f32 currentTime)
{
	if (_scaleKeys.empty())
		return mat4f(1.0f);
	if (_scaleKeys.size() == 1)
		return glm::scale(mat4f(1.0f), _scaleKeys[0].scale);

	const auto [currentKey, nextKey] = FindCurrentScaleKey(currentTime);
	f32 scaleFactor = CalculateBlendFactor(currentKey->timeStamp, nextKey->timeStamp,currentTime);
	vec3f finalScale = glm::mix(currentKey->scale, nextKey->scale, scaleFactor);
	return glm::scale(mat4f(1.0f), finalScale);
}

std::pair<KeyPosition*, KeyPosition*> Bone::FindCurrentPositionKey(f32 currentTime)
{
	for (u64 i = 0; i < _positionKeys.size() - 1; i++)
		if (currentTime < _positionKeys[i + 1].timeStamp)
			return { &_positionKeys[i], &_positionKeys[i + 1] };
	return { nullptr, nullptr };
}
std::pair<KeyRotation*, KeyRotation*> Bone::FindCurrentRotationKey(f32 currentTime)
{
	for (u64 i = 0; i < _rotationKeys.size() - 1; i++)
		if (currentTime < _rotationKeys[i + 1].timeStamp)
			return { &_rotationKeys[i], &_rotationKeys[i + 1] };
	return { nullptr, nullptr };
}
std::pair<KeyScale*, KeyScale*> Bone::FindCurrentScaleKey(f32 currentTime)
{
	for (u64 i = 0; i < _scaleKeys.size() - 1; i++)
		if (currentTime < _scaleKeys[i + 1].timeStamp)
			return { &_scaleKeys[i], &_scaleKeys[i + 1] };
	return { nullptr, nullptr };
}

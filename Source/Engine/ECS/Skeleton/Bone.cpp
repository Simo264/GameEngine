#include "Bone.hpp"

#include "Core/Math/Ext.hpp"

#include <assimp/scene.h>

/* ---------------------------------------------------- */
/*										PUBLIC														*/
/* ---------------------------------------------------- */

Bone::Bone(StringView name, i32 id, const aiNodeAnim* channel) :
	_name{ name.data() },
	_id{ id },
	_positionKeys{},
	_rotationKeys{},
	_scaleKeys{},
	_numPositionKeys{ 0 },
	_numRotationKeys{ 0 },
	_numScalingKeys{ 0 },
	_localTransform{}
{
	_numPositionKeys = channel->mNumPositionKeys;
	for (i32 positionIndex = 0; positionIndex < _numPositionKeys; positionIndex++)
	{
		aiVector3D aiPosition = channel->mPositionKeys[positionIndex].mValue;
		vec3f position = vec3f(aiPosition.x, aiPosition.y, aiPosition.z);
		f32 timeStamp = channel->mPositionKeys[positionIndex].mTime;
		_positionKeys.emplace_back(position, timeStamp);
	}
	
	_numRotationKeys = channel->mNumRotationKeys;
	for (i32 rotationIndex = 0; rotationIndex < _numRotationKeys; rotationIndex++)
	{
		aiQuaternion aiOrientation = channel->mRotationKeys[rotationIndex].mValue;
		quat orientation = quat(aiOrientation.w, aiOrientation.x, aiOrientation.y, aiOrientation.z);
		f32 timeStamp = channel->mRotationKeys[rotationIndex].mTime;
		_rotationKeys.emplace_back(orientation, timeStamp);
	}

	_numScalingKeys = channel->mNumScalingKeys;
	for (i32 keyIndex = 0; keyIndex < _numScalingKeys; ++keyIndex)
	{
		aiVector3D aiscale = channel->mScalingKeys[keyIndex].mValue;
		vec3f scale = vec3f(aiscale.x, aiscale.y, aiscale.z);
		f32 timeStamp = channel->mScalingKeys[keyIndex].mTime;
		_scaleKeys.emplace_back(scale, timeStamp);
	}
}

void Bone::Update(f32 animationTime)
{
	/**
	 * Interpolates  b/w positions,rotations & scaling keys based on the curren time of
	 * the animation and prepares the local transformation matrix by combining all keys
	 * tranformations
	 */

	mat4f translation = InterpolatePosition(animationTime);
	mat4f rotation		= InterpolateRotation(animationTime);
	mat4f scale				= InterpolateScaling(animationTime);
	_localTransform		= translation * rotation * scale;
}

i32 Bone::GetPositionIndex(f32 animationTime)
{
	/**
	 * Gets the current index on mKeyPositions to interpolate to based on
	 * the current animation time
	 */

	for (i32 i = 0; i < _numPositionKeys - 1; i++)
		if (animationTime < _positionKeys.at(static_cast<u64>(i) + 1).timeStamp)
			return i;
	
	return -1;
}

i32 Bone::GetRotationIndex(f32 animationTime)
{
	/**
	 * Gets the current index on mKeyRotations to interpolate to based on the 
	 * current animation time.
	 */

	for (i32 i = 0; i < _numRotationKeys - 1; i++)
		if (animationTime < _rotationKeys.at(static_cast<u64>(i) + 1).timeStamp)
			return i;

	return -1;
}

i32 Bone::GetScaleIndex(f32 animationTime)
{
	/**
	 * Gets the current index on mKeyScalings to interpolate to based on the
	 * current animation time
	 */
	
	for (i32 i = 0; i < _numScalingKeys - 1; i++)
		if (animationTime < _scaleKeys.at(static_cast<u64>(i) + 1).timeStamp)
			return i;

	return -1;
}

/* ---------------------------------------------------- */
/*										PRIVATE														*/
/* ---------------------------------------------------- */

f32 Bone::GetScaleFactor(f32 lastTimeStamp, f32 nextTimeStamp, f32 animationTime)
{
	/* Gets normalized value for Lerp & Slerp */
	f32 midWayLength = animationTime - lastTimeStamp;
	f32 framesDiff = nextTimeStamp - lastTimeStamp;
	return midWayLength / framesDiff;
}

mat4f Bone::InterpolatePosition(f32 animationTime)
{
	/**
	 * Figures out which position keys to interpolate b/w and performs the interpolation 
	 * and returns the translation matrix.
	 */

	if (_numPositionKeys == 1)
		return glm::translate(mat4f(1.0f), _positionKeys.at(0).position);

	i32 p0Index = GetPositionIndex(animationTime);
	i32 p1Index = p0Index + 1;
	f32 scaleFactor = GetScaleFactor(_positionKeys.at(p0Index).timeStamp, _positionKeys.at(p1Index).timeStamp, animationTime);
	vec3f finalPosition = glm::mix(_positionKeys.at(p0Index).position, _positionKeys.at(p1Index).position, scaleFactor);
	return glm::translate(mat4f(1.0f), finalPosition);
}

mat4f Bone::InterpolateRotation(f32 animationTime)
{
	if (_numRotationKeys == 1)
	{
		quat rotation = glm::normalize(_rotationKeys.at(0).orientation);
		return glm::mat4_cast(rotation);
	}

	i32 p0Index = GetRotationIndex(animationTime);
	i32 p1Index = p0Index + 1;
	f32 scaleFactor = GetScaleFactor(_rotationKeys.at(p0Index).timeStamp, _rotationKeys.at(p1Index).timeStamp, animationTime);
	quat finalRotation = glm::slerp(_rotationKeys.at(p0Index).orientation, _rotationKeys.at(p1Index).orientation, scaleFactor);
	finalRotation = glm::normalize(finalRotation);
	return glm::mat4_cast(finalRotation);
}

mat4f Bone::InterpolateScaling(f32 animationTime)
{
	if (_numScalingKeys == 1)
		return glm::scale(mat4f(1.0f), _scaleKeys.at(0).scale);

	i32 p0Index = GetScaleIndex(animationTime);
	i32 p1Index = p0Index + 1;
	f32 scaleFactor = GetScaleFactor(_scaleKeys.at(p0Index).timeStamp, _scaleKeys.at(p1Index).timeStamp, animationTime);
	vec3f finalScale = glm::mix(_scaleKeys.at(p0Index).scale, _scaleKeys.at(p1Index).scale, scaleFactor);
	return glm::scale(mat4f(1.0f), finalScale);
}

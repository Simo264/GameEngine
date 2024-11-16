#include "Bone.hpp"

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

Bone::Bone(StringView name, i32 id, const aiNodeAnim* channel) :	
	_name{ name.data() },
	_id{ id },
	_positions{},
	_rotations{},
	_scales{},
	_localTransform{}
{
	u32 numPositions = channel->mNumPositionKeys;
	_positions.reserve(numPositions);
	for (u32 i = 0; i < numPositions; i++)
	{
		const aiVector3D& aiPosition = channel->mPositionKeys[i].mValue;
		vec3f position = vec3f(aiPosition.x, aiPosition.y, aiPosition.z);
		f32 timeStamp = channel->mPositionKeys[i].mTime;
		_positions.emplace_back(position, timeStamp);
	}

	u32 numRotations = channel->mNumRotationKeys;
	_rotations.reserve(numRotations);
	for (u32 i = 0; i < numRotations; i++)
	{
		const aiQuaternion& aiOrientation = channel->mRotationKeys[i].mValue;
		quat orientation = quat(aiOrientation.w, aiOrientation.x, aiOrientation.y, aiOrientation.z);
		f32 timeStamp = channel->mRotationKeys[i].mTime;
		_rotations.emplace_back(orientation, timeStamp);
	}

	u32 numScalings = channel->mNumScalingKeys;
	_scales.reserve(numScalings);
	for (u32 i = 0; i < numScalings; i++)
	{
		const aiVector3D& aiscale = channel->mScalingKeys[i].mValue;
		vec3f scale = vec3f(aiscale.x, aiscale.y, aiscale.z);
		f32 timeStamp = channel->mScalingKeys[i].mTime;
		_scales.emplace_back(scale, timeStamp);
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

	u64 numPositions = _positions.size();
	for (u64 i = 0; i < numPositions - 1; i++)
		if (animationTime < _positions.at(i + 1).timeStamp)
			return i;
	
	return -1;
}

i32 Bone::GetRotationIndex(f32 animationTime)
{
	/**
	 * Gets the current index on mKeyRotations to interpolate to based on the
	 * current animation time.
	 */

	u64 numRotations = _rotations.size();
	for (u64 i = 0; i < numRotations - 1; i++)
		if (animationTime < _rotations.at(i + 1).timeStamp)
			return i;
	
	return -1;
}

i32 Bone::GetScaleIndex(f32 animationTime)
{
	/**
	 * Gets the current index on mKeyScalings to interpolate to based on the
	 * current animation time
	 */

	u64 numScalings = _scales.size();
	for (u64 i = 0; i < numScalings - 1; i++)
		if (animationTime < _scales.at(i + 1).timeStamp)
			return i;
	
	return -1;
}


/* ---------------------------------------------------- */
/*										PRIVATE														*/
/* ---------------------------------------------------- */


f32 Bone::GetScaleFactor(f32 lastTimeStamp, f32 nextTimeStamp, f32 animationTime)
{
	f32 midWayLength = animationTime - lastTimeStamp;
	f32 framesDiff = nextTimeStamp - lastTimeStamp;
	return midWayLength / framesDiff;
}

mat4f Bone::InterpolatePosition(f32 animationTime)
{
	if (_positions.size() == 1)
		return glm::translate(mat4f(1.0f), _positions.at(0).position);

	i32 p0Index = GetPositionIndex(animationTime);
	i32 p1Index = p0Index + 1;
	f32 scaleFactor = GetScaleFactor(_positions.at(p0Index).timeStamp, _positions.at(p1Index).timeStamp, animationTime);
	vec3f finalPosition = glm::mix(_positions.at(p0Index).position, _positions.at(p1Index).position, scaleFactor);
	return glm::translate(mat4f(1.0f), finalPosition);
}

mat4f Bone::InterpolateRotation(f32 animationTime)
{
	if (_rotations.size() == 1)
	{
		quat rotation = glm::normalize(_rotations.at(0).orientation);
		return glm::mat4_cast(rotation);
	}

	i32 p0Index = GetRotationIndex(animationTime);
	i32 p1Index = p0Index + 1;
	f32 scaleFactor = GetScaleFactor(_rotations.at(p0Index).timeStamp, _rotations.at(p1Index).timeStamp, animationTime);
	quat finalRotation = glm::slerp(_rotations.at(p0Index).orientation, _rotations.at(p1Index).orientation, scaleFactor);
	finalRotation = glm::normalize(finalRotation);
	return glm::mat4_cast(finalRotation);

}

mat4f Bone::InterpolateScaling(f32 animationTime)
{
	if (_scales.size() == 1)
		return glm::scale(mat4f(1.0f), _scales.at(0).scale);

	i32 p0Index = GetScaleIndex(animationTime);
	i32 p1Index = p0Index + 1;
	f32 scaleFactor = GetScaleFactor(_scales.at(p0Index).timeStamp, _scales.at(p1Index).timeStamp, animationTime);
	vec3f finalScale = glm::mix(_scales.at(p0Index).scale, _scales.at(p1Index).scale, scaleFactor);
	return glm::scale(mat4f(1.0f), finalScale);
}
#include "Bone.hpp"

#include <assimp/scene.h>

static mat4f ConvertMatrixToGLMFormat(const aiMatrix4x4& from)
{
	mat4f to{};
	//the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
	to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
	to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
	to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
	to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
	return to;
}
static vec3f GetGLMVec(const aiVector3D& vec)
{
	return vec3f(vec.x, vec.y, vec.z);
}
static quat GetGLMQuat(const aiQuaternion& pOrientation)
{
	return quat(pOrientation.w, pOrientation.x, pOrientation.y, pOrientation.z);
}

/* ---------------------------------------------------- */
/*										PUBLIC														*/
/* ---------------------------------------------------- */

Bone::Bone(const String& name, i32 ID, const aiNodeAnim* channel)
	: m_Name(name),
		m_ID(ID),
		m_LocalTransform(1.0f)
{
	m_NumPositions = channel->mNumPositionKeys;
	for (int positionIndex = 0; positionIndex < m_NumPositions; ++positionIndex)
	{
		aiVector3D aiPosition = channel->mPositionKeys[positionIndex].mValue;
		float timeStamp = channel->mPositionKeys[positionIndex].mTime;
		KeyPosition data;
		data.position = GetGLMVec(aiPosition);
		data.timeStamp = timeStamp;
		m_Positions.push_back(data);
	}

	m_NumRotations = channel->mNumRotationKeys;
	for (int rotationIndex = 0; rotationIndex < m_NumRotations; ++rotationIndex)
	{
		aiQuaternion aiOrientation = channel->mRotationKeys[rotationIndex].mValue;
		float timeStamp = channel->mRotationKeys[rotationIndex].mTime;
		KeyRotation data;
		data.orientation = GetGLMQuat(aiOrientation);
		data.timeStamp = timeStamp;
		m_Rotations.push_back(data);
	}

	m_NumScalings = channel->mNumScalingKeys;
	for (int keyIndex = 0; keyIndex < m_NumScalings; ++keyIndex)
	{
		aiVector3D scale = channel->mScalingKeys[keyIndex].mValue;
		float timeStamp = channel->mScalingKeys[keyIndex].mTime;
		KeyScale data;
		data.scale = GetGLMVec(scale);
		data.timeStamp = timeStamp;
		m_Scales.push_back(data);
	}
}

void Bone::Update(f32 animationTime)
{
	mat4f translation = InterpolatePosition(animationTime);
	mat4f rotation = InterpolateRotation(animationTime);
	mat4f scale = InterpolateScaling(animationTime);
	m_LocalTransform = translation * rotation * scale;
}

i32 Bone::GetPositionIndex(f32 animationTime)
{
	for (int index = 0; index < m_NumPositions - 1; ++index)
	{
		if (animationTime < m_Positions[index + 1].timeStamp)
			return index;
	}
	assert(0);
	return -1;
}

i32 Bone::GetRotationIndex(f32 animationTime)
{
	for (int index = 0; index < m_NumRotations - 1; ++index)
	{
		if (animationTime < m_Rotations[index + 1].timeStamp)
			return index;
	}
	assert(0);
	return -1;
}

i32 Bone::GetScaleIndex(f32 animationTime)
{
	for (int index = 0; index < m_NumScalings - 1; ++index)
	{
		if (animationTime < m_Scales[index + 1].timeStamp)
			return index;
	}
	assert(0);
	return -1;
}






//Bone::Bone(StringView name, i32 id, const aiNodeAnim* channel) :
//	_name{ name.data() },
//	_id{ id },
//	_positionKeys{},
//	_rotationKeys{},
//	_scaleKeys{},
//	_numPositionKeys{ 0 },
//	_numRotationKeys{ 0 },
//	_numScalingKeys{ 0 },
//	_localTransform{}
//{
//	_numPositionKeys = channel->mNumPositionKeys;
//	for (i32 positionIndex = 0; positionIndex < _numPositionKeys; positionIndex++)
//	{
//		aiVector3D aiPosition = channel->mPositionKeys[positionIndex].mValue;
//		vec3f position = vec3f(aiPosition.x, aiPosition.y, aiPosition.z);
//		f32 timeStamp = channel->mPositionKeys[positionIndex].mTime;
//		_positionKeys.emplace_back(position, timeStamp);
//	}
//	
//	_numRotationKeys = channel->mNumRotationKeys;
//	for (i32 rotationIndex = 0; rotationIndex < _numRotationKeys; rotationIndex++)
//	{
//		aiQuaternion aiOrientation = channel->mRotationKeys[rotationIndex].mValue;
//		quat orientation = quat(aiOrientation.w, aiOrientation.x, aiOrientation.y, aiOrientation.z);
//		f32 timeStamp = channel->mRotationKeys[rotationIndex].mTime;
//		_rotationKeys.emplace_back(orientation, timeStamp);
//	}
//
//	_numScalingKeys = channel->mNumScalingKeys;
//	for (i32 keyIndex = 0; keyIndex < _numScalingKeys; ++keyIndex)
//	{
//		aiVector3D aiscale = channel->mScalingKeys[keyIndex].mValue;
//		vec3f scale = vec3f(aiscale.x, aiscale.y, aiscale.z);
//		f32 timeStamp = channel->mScalingKeys[keyIndex].mTime;
//		_scaleKeys.emplace_back(scale, timeStamp);
//	}
//}
//
//void Bone::Update(f32 animationTime)
//{
//	/**
//	 * Interpolates  b/w positions,rotations & scaling keys based on the curren time of
//	 * the animation and prepares the local transformation matrix by combining all keys
//	 * tranformations
//	 */
//
//	mat4f translation = InterpolatePosition(animationTime);
//	mat4f rotation		= InterpolateRotation(animationTime);
//	mat4f scale				= InterpolateScaling(animationTime);
//	_localTransform		= translation * rotation * scale;
//}
//
//i32 Bone::GetPositionIndex(f32 animationTime)
//{
//	/**
//	 * Gets the current index on mKeyPositions to interpolate to based on
//	 * the current animation time
//	 */
//
//	for (i32 i = 0; i < _numPositionKeys - 1; i++)
//		if (animationTime < _positionKeys.at(static_cast<u64>(i) + 1).timeStamp)
//			return i;
//	
//	return -1;
//}
//
//i32 Bone::GetRotationIndex(f32 animationTime)
//{
//	/**
//	 * Gets the current index on mKeyRotations to interpolate to based on the 
//	 * current animation time.
//	 */
//
//	for (i32 i = 0; i < _numRotationKeys - 1; i++)
//		if (animationTime < _rotationKeys.at(static_cast<u64>(i) + 1).timeStamp)
//			return i;
//
//	return -1;
//}
//
//i32 Bone::GetScaleIndex(f32 animationTime)
//{
//	/**
//	 * Gets the current index on mKeyScalings to interpolate to based on the
//	 * current animation time
//	 */
//	
//	for (i32 i = 0; i < _numScalingKeys - 1; i++)
//		if (animationTime < _scaleKeys.at(static_cast<u64>(i) + 1).timeStamp)
//			return i;
//
//	return -1;
//}

/* ---------------------------------------------------- */
/*										PRIVATE														*/
/* ---------------------------------------------------- */


f32 Bone::GetScaleFactor(f32 lastTimeStamp, f32 nextTimeStamp, f32 animationTime)
{
	f32 scaleFactor = 0.0f;
	f32 midWayLength = animationTime - lastTimeStamp;
	f32 framesDiff = nextTimeStamp - lastTimeStamp;
	scaleFactor = midWayLength / framesDiff;
	return scaleFactor;
}

mat4f Bone::InterpolatePosition(f32 animationTime)
{
	if (1 == m_NumPositions)
		return glm::translate(glm::mat4(1.0f), m_Positions[0].position);

	i32 p0Index = GetPositionIndex(animationTime);
	i32 p1Index = p0Index + 1;
	f32 scaleFactor = GetScaleFactor(m_Positions[p0Index].timeStamp, m_Positions[p1Index].timeStamp, animationTime);
	glm::vec3 finalPosition = glm::mix(m_Positions[p0Index].position, m_Positions[p1Index].position, scaleFactor);
	return glm::translate(glm::mat4(1.0f), finalPosition);
}

mat4f Bone::InterpolateRotation(f32 animationTime)
{
	if (1 == m_NumRotations)
	{
		auto rotation = glm::normalize(m_Rotations[0].orientation);
		return glm::toMat4(rotation);
	}

	i32 p0Index = GetRotationIndex(animationTime);
	i32 p1Index = p0Index + 1;
	f32 scaleFactor = GetScaleFactor(m_Rotations[p0Index].timeStamp,m_Rotations[p1Index].timeStamp, animationTime);
	glm::quat finalRotation = glm::slerp(m_Rotations[p0Index].orientation, m_Rotations[p1Index].orientation, scaleFactor);
	finalRotation = glm::normalize(finalRotation);
	return glm::toMat4(finalRotation);

}

mat4f Bone::InterpolateScaling(f32 animationTime)
{
	if (1 == m_NumScalings)
		return glm::scale(glm::mat4(1.0f), m_Scales[0].scale);

	i32 p0Index = GetScaleIndex(animationTime);
	i32 p1Index = p0Index + 1;
	f32 scaleFactor = GetScaleFactor(m_Scales[p0Index].timeStamp, m_Scales[p1Index].timeStamp, animationTime);
	vec3f finalScale = glm::mix(m_Scales[p0Index].scale, m_Scales[p1Index].scale, scaleFactor);
	return glm::scale(glm::mat4(1.0f), finalScale);
}










//f32 Bone::GetScaleFactor(f32 lastTimeStamp, f32 nextTimeStamp, f32 animationTime)
//{
//	/* Gets normalized value for Lerp & Slerp */
//	f32 midWayLength = animationTime - lastTimeStamp;
//	f32 framesDiff = nextTimeStamp - lastTimeStamp;
//	return midWayLength / framesDiff;
//}
//
//mat4f Bone::InterpolatePosition(f32 animationTime)
//{
//	/**
//	 * Figures out which position keys to interpolate b/w and performs the interpolation 
//	 * and returns the translation matrix.
//	 */
//
//	if (_numPositionKeys == 1)
//		return glm::translate(mat4f(1.0f), _positionKeys.at(0).position);
//
//	i32 p0Index = GetPositionIndex(animationTime);
//	i32 p1Index = p0Index + 1;
//	f32 scaleFactor = GetScaleFactor(_positionKeys.at(p0Index).timeStamp, _positionKeys.at(p1Index).timeStamp, animationTime);
//	vec3f finalPosition = glm::mix(_positionKeys.at(p0Index).position, _positionKeys.at(p1Index).position, scaleFactor);
//	return glm::translate(mat4f(1.0f), finalPosition);
//}
//
//mat4f Bone::InterpolateRotation(f32 animationTime)
//{
//	if (_numRotationKeys == 1)
//	{
//		quat rotation = glm::normalize(_rotationKeys.at(0).orientation);
//		return glm::mat4_cast(rotation);
//	}
//
//	i32 p0Index = GetRotationIndex(animationTime);
//	i32 p1Index = p0Index + 1;
//	f32 scaleFactor = GetScaleFactor(_rotationKeys.at(p0Index).timeStamp, _rotationKeys.at(p1Index).timeStamp, animationTime);
//	quat finalRotation = glm::slerp(_rotationKeys.at(p0Index).orientation, _rotationKeys.at(p1Index).orientation, scaleFactor);
//	finalRotation = glm::normalize(finalRotation);
//	return glm::mat4_cast(finalRotation);
//}
//
//mat4f Bone::InterpolateScaling(f32 animationTime)
//{
//	if (_numScalingKeys == 1)
//		return glm::scale(mat4f(1.0f), _scaleKeys.at(0).scale);
//
//	i32 p0Index = GetScaleIndex(animationTime);
//	i32 p1Index = p0Index + 1;
//	f32 scaleFactor = GetScaleFactor(_scaleKeys.at(p0Index).timeStamp, _scaleKeys.at(p1Index).timeStamp, animationTime);
//	vec3f finalScale = glm::mix(_scaleKeys.at(p0Index).scale, _scaleKeys.at(p1Index).scale, scaleFactor);
//	return glm::scale(mat4f(1.0f), finalScale);
//}

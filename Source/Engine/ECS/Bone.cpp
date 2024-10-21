#include "Bone.hpp"

#include <assimp/scene.h>

/* ---------------------------------------------------- */
/*										PUBLIC														*/
/* ---------------------------------------------------- */

Bone::Bone(StringView name, i32 id, const aiNodeAnim* channel)
{

}

void Bone::Update(f32 animationTime)
{

}

i32 Bone::GetPositionIndex(f32 animationTime)
{

}

i32 Bone::GetRotationIndex(f32 animationTime)
{

}

i32 Bone::GetScaleIndex(f32 animationTime)
{

}

/* ---------------------------------------------------- */
/*										PRIVATE														*/
/* ---------------------------------------------------- */

f32 Bone::GetScaleFactor(f32 lastTimeStamp, f32 nextTimeStamp, f32 animationTime)
{

}

mat4f Bone::InterpolatePosition(f32 animationTime)
{

}

mat4f Bone::InterpolateRotation(f32 animationTime)
{

}

mat4f Bone::InterpolateScaling(f32 animationTime)
{

}

#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Core.hpp"

struct aiNodeAnim;

struct BoneInfo
{
  /* ID is index in finalBoneMatrices */
  i32 id;
  /* Offset matrix transforms vertex from model space to bone space */
  mat4f offset;
};

/**
 * @brief 
 * Represents a position in 3D space at a given time instant (timeStamp). 
 * It is used to interpolate positions during animation.
 * 
 * Each KeyPosition represents a 'keyframe' of a particular position of a bone in an animation. 
 * Interpolation between several KeyPositions allows the model to move smoothly from one position to another.
 */
struct KeyPosition
{
  vec3f position;
  f32 timeStamp;
};

/**
 * @brief 
 * Represents the rotation of a bone at a given instant in time, 
 * using a quaternion (quat) to handle the rotation. 
 * Quaternions are ideal for representing rotations without running into the problems of gimbal lock 
 * that can be encountered with Euler angles.
 * 
 * Each KeyRotation defines a bone rotation in a keyframe. 
 * Spherical Linear Interpolation (slerp) between quaternions allows a smooth transition between two rotations.
 */
struct KeyRotation
{
  quat orientation;
  f32 timeStamp;
};

/**
 * @brief 
 * Represents the scaling of a bone at a given instant in time. 
 * Scaling makes it possible to vary the size of a bone, and consequently 
 * the part of the mesh associated with that bone.
 * 
 * It is interpolated during animation to grow or shrink the bone (and thus the parts of the mesh).
 */
struct KeyScale
{
  vec3f scale;
  f32 timeStamp;
};

/**
 * @brief 
 * The Bone class handles interpolated animations of a single bone. 
 * During rendering, the animation system interpolates position, rotation and 
 * scale transformations based on the current animation time, and then combines 
 * these values to generate the final transformation matrix for that bone.
 * 
 * Each bone will have a set of keyframes for position, rotation and scale, 
 * and these will be interpolated to create fluid movements.
 * 
 * To achieve smooth animation between keyframes, position, rotation and scale are interpolated. 
 * Positions and scales are interpolated linearly, while rotations (quaternions) are interpolated using slerps.
 * 
 * Bones can be organised in a hierarchical structure, where a child bone inherits 
 * the transformations of its parent bone.
 */
class Bone
{
public:
  Bone(StringView name, i32 id, const aiNodeAnim* channel);

  /**
   * @brief 
   * This method updates the local bone transformation according to the time 
   * of the current animation (animationTime).
   * 
   * This method is called upon during rendering to update the position, rotation and scale 
   * of the bone in an interpolated manner. 
   * It relies on the interpolation of the position, rotation and scale keyframes 
   * to calculate the total transformation matrix.
   */
  void Update(f32 animationTime);

  /**
   * @brief
   * Used to determine between which two keyframes to interpolate in order to calculate 
   * the exact position of the bone at theanimationTime. 
   * The nearest keyframe that has a timestamp less than or equal to the animationTime is searched.
   * 
   * @return
   * The index of the position keyframe that is immediately preceding the current animationTime.
   */
  i32 GetPositionIndex(f32 animationTime);

  /**
   * @brief
   * Works like GetPositionIndex but for rotation. 
   * The system will search for the keyframe preceding the current time to interpolate the correct rotation.
   * 
   * @return
   * The index of the rotation keyframe immediately preceding the current animationTime.
   */
  i32 GetRotationIndex(f32 animationTime);

  /**
   * @brief
   * Similar to the previous methods, this finds the appropriate scaling keyframe from which to start the interpolation.
   * 
   * @return
   * The index of the scaling keyframe immediately preceding the current animationTime.
   */
  i32 GetScaleIndex(f32 animationTime);

  const String& GetName() const { return _name; }
  const mat4f& GetLocalTransform() const { return _localTransform; }

private:
  f32 GetScaleFactor(f32 lastTimeStamp, f32 nextTimeStamp, f32 animationTime);
  mat4f InterpolatePosition(f32 animationTime);
  mat4f InterpolateRotation(f32 animationTime);
  mat4f InterpolateScaling(f32 animationTime);

  String _name;
  i32 _id;

  Vector<KeyPosition> _positionKeys;
  Vector<KeyRotation> _rotationKeys;
  Vector<KeyScale>    _scaleKeys;
  i32 _numPositionKeys;
  i32 _numRotationKeys;
  i32 _numScalingKeys;

  mat4f _localTransform;
};

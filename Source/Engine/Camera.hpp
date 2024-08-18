#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Math.hpp"

struct Frustum {
	f32 left{ -10.0f };
	f32 right{ 10.0f };
	f32 bottom{ -10.0f };
	f32 top{ 10.0f };
	f32 zNear{ 0.1f };
	f32 zFar{ 20.0f };
};

/**
 * When we're talking about camera/view space we're talking about all the vertex
 * coordinates as seen from the camera's perspective as the origin of the scene:
 * the view matrix transforms all the world coordinates into view coordinates
 * that are relative to the camera's position and direction.
 * To define a camera we need its position in world space, the direction
 * it's looking at, a vector pointing to the right and a vector pointing upwards
 * from the camera.
 * A careful reader may notice that we're actually going to create a coordinate
 * system with 3 perpendicular unit axes with the camera's position as the origin.
 */
class Camera
{
public:
	Camera(
		vec3f position,
		f32 yaw = -90.0f,
		f32 pitch = 0.0f,
		f32 roll = 0.0f
	);
	~Camera() = default;

	vec3f position;

	/* Euler angles (in degrees) */
	f32 yaw;
	f32 pitch;
	f32 roll;

	f32 fov;

	Frustum frustum;

	const vec3f& GetFrontVector() { return _front; }
	const vec3f& GetRightVector() { return _right; }
	const vec3f& GetUpVector() { return _up; }

	/**
	 * Update front, up and right vectors
	 */
	void UpdateOrientation();

	/**
	 * Calculate the view matrix
	 *
	 * @param center: position where the camera is looking at
	 */
	mat4f CalculateView(vec3f center) const;

	/**
	 * Calculate the projection matrix
	 */
	mat4f CalculatePerspective(f32 aspect) const;

	/**
	 * Calculate the projection matrix
	 */
	mat4f CalculateOrtho() const;

	void ProcessKeyboard(f32 delta, f32 movementSpeed);
	void ProcessMouse(f32 delta, f32 mouseSensitivity);

private:
	/* Orientation vectors */
	vec3f _front;
	vec3f _up;
	vec3f _right;
};
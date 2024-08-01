#pragma once

#include "Core/Math/Math.hpp"

struct Frustum {
	float left{ -10.0f };
	float right{ 10.0f };
	float bottom{ -10.0f };
	float top{ 10.0f };
	float zNear{ 0.1f };
	float zFar{ 20.0f };
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
		float yaw = -90.0f,
		float pitch = 0.0f,
		float roll = 0.0f
	);
	~Camera() = default;

	vec3f position;

	/* Euler angles (in degrees) */
	float yaw;
	float pitch;
	float roll;

	float fov;

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
	mat4f CalculatePerspective(float aspect) const;

	/**
	 * Calculate the projection matrix
	 */
	mat4f CalculateOrtho() const;

	void ProcessKeyboard(float delta);
	void ProcessMouse(float delta);

private:
	/* Orientation vectors */
	vec3f _front;
	vec3f _up;
	vec3f _right;
};
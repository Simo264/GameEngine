#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Core.hpp"

/**
 * @struct Frustum
 * 
 * @brief Defines the viewing frustum for a camera.
 */
struct Frustum 
{
	/** @brief Left plane of the frustum */
	f32 left{ -10.0f };
	/** @brief Right plane of the frustum */
	f32 right{ 10.0f };
	/** @brief Bottom plane of the frustum */
	f32 bottom{ -10.0f };
	/** @brief Top plane of the frustum */
	f32 top{ 10.0f };
	/** @brief Near plane distance */
	f32 zNear{ 0.1f };
	/** @brief Far plane distance */
	f32 zFar{ 20.0f };
};

/**
 * @class Camera
 * 
 * @brief Represents a 3D camera with position, orientation, and projection capabilities
 */
class Camera
{
public:
	/**
	 * @brief Constructs a Camera object with specified position, orientation, and field of view.
	 * 
	 * @param position Initial position of the camera (default is vec3f(0.f)).
	 * @param orientation Initial orientation of the camera as Euler angles (yaw, pitch, roll).
	 * @param fov Field of view in degrees (default is 45.f)
	 */
	Camera(
		vec3f position		= vec3f(0.f),
		vec3f orientation = vec3f(-90.f, 0.f, 0.f),
		f32 fov						= 45.f
	);
	/** @brief Default destructor for Camera */
	~Camera() = default;

	/** @brief Camera position (x, y, z) */
	vec3f position;
	/** @brief Camera orientation as Euler angles (yaw, pitch, roll). */
	vec3f orientation;
	/** @brief Field of view in degrees. */
	f32 fov;
	/** @brief Viewing frustum of the camera */
	Frustum frustum;

	/** @brief Gets the front vector of the camera. */
	const vec3f& GetFrontVector() { return _front; }
	/** @brief Gets the right vector of the camera. */
	const vec3f& GetRightVector() { return _right; }
	/** @brief Gets the up vector of the camera. */
	const vec3f& GetUpVector() { return _up; }

	/** @brief Updates the orientation of the camera by recalculating the front, up, and right vectors. */
	void UpdateOrientation();

	/**
	 * @brief Calculates the view matrix for the camera.
	 * 
	 * @param center Position where the camera is looking at.
	 * 
	 * @return View matrix as a mat4f.
	 */
	mat4f CalculateView(vec3f center) const;

	/**
	 * @brief Calculates the perspective projection matrix for the camera.
	 *
	 * @param aspect Aspect ratio of the viewport.
	 *
	 * @return Perspective projection matrix as a mat4f
	 */
	mat4f CalculatePerspective(f32 aspect) const;

	/**
	 * @brief Calculates the orthographic projection matrix for the camera.
	 *
	 * @return Orthographic projection matrix as a mat4f.
	 */
	mat4f CalculateOrtho() const;

	/** @brief Processes keyboard input to move the camera. */
	void ProcessKeyboard(f32 delta, f32 movementSpeed);
	/** @brief Processes mouse input to adjust the camera orientation. */
	void ProcessMouse(f32 delta, f32 mouseSensitivity);

private:
	vec3f _front; // Front vector representing the direction the camera is facing
	vec3f _up;		// Up vector representing the upward direction
	vec3f _right; // Right vector perpendicular to the front and up vectors.
};
#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Base.hpp"

class Camera
{
public:
	Camera();
	~Camera() = default;

	/** @brief Computes the view matrix */
	Mat4f GetViewMatrix() const;

	/**
	 * @brief Computes a perspective projection matrix
	 *
	 * @param fovy   Vertical field of view in radians
	 * @param aspect Aspect ratio of the viewport
	 * @param n      Distance to the near clipping plane (positive)
	 * @param f      Distance to the far clipping plane (positive)
	 */
	Mat4f GetPerspectiveProjection(f32 fovy, f32 aspect, f32 n, f32 f) const;
	
	/**
	 * @brief Computes an orthographic projection matrix
	 *
	 * @param l Left boundary of the view volume
	 * @param r Right boundary of the view volume
	 * @param b Bottom boundary of the view volume
	 * @param t Top boundary of the view volume
	 * @param n Distance to the near clipping plane (positive)
	 * @param f Distance to the far clipping plane (positive)
	 */
	Mat4f GetOrthographicProjection(f32 l, f32 r,
																	f32 b, f32 t,
																	f32 n, f32 f) const;

	void ProcessKeyboard(f32 delta, f32 movementSpeed);
	void ProcessMouse(f32 delta, f32 mouseSensitivity);

	Vec3F GetForwardVector() const { return Quaternion(eulerAngles) * Vec3F(0, 0, -1); }
	Vec3F GetRightVector() const { return Quaternion(eulerAngles) * Vec3F(1, 0, 0); }
	Vec3F GetUpVector() const { return Quaternion(eulerAngles) * Vec3F(0, 1, 0); }

	Vec3F position;			// the eye position vector
	Vec3F eulerAngles;	
};
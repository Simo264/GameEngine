#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Base.hpp"

namespace Components
{
	struct Camera
	{
		/** @brief Computes the view matrix */
		Mat4F GetViewMatrix() const;
		/** @brief Computes a perspective projection matrix */
		Mat4F GetPerspectiveProjection(f32 fovy, f32 aspect, f32 n, f32 f) const;
		/** @brief Computes an orthographic projection matrix */
		Mat4F GetOrthographicProjection(f32 l, f32 r, f32 b, f32 t, f32 n, f32 f) const;
		
		Vec3F CalculateForwardVector() const;
		Vec3F CalculateRightVector() const;
		Vec3F CalculateUpVector() const;

		void ProcessKeyboard(f32 delta, f32 movementSpeed);
		void ProcessMouseMovement(f32 mouseSensitivity);

		Vec3F position{ 0.f };	// the eye position vector
		Vec3F eulerAngles{ 0.f };
	};
}


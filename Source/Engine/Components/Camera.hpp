#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Base.hpp"

namespace Components
{
	struct Camera
	{
		/** @brief Computes the view matrix */
		Mat4F CalculateViewMatrix() const;
		/** @brief Computes a perspective projection matrix */
		Mat4F CalculatePerspectiveMatrix(f32 aspect) const;
		/** @brief Computes an orthographic projection matrix */
		Mat4F CalculateOrthographicMatrix(f32 l, f32 r, f32 b, f32 t) const;
		
		Vec3F CalculateForwardVector() const;
		Vec3F CalculateRightVector() const;
		Vec3F CalculateUpVector() const;

		void ProcessKeyboard(f32 delta, f32 movementSpeed);
		void ProcessMouseMovement(f32 mouseSensitivity);

		Vec3F position{ 0.f };
		Vec3F eulerAngles{ 0.f };
		f32 fovH{ 60.f }; // in degrees
		f32 nearClip{ 0.1f };
		f32 farClip{ 50.0f };
	};
}


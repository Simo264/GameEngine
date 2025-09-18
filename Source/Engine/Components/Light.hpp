#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Base.hpp"

enum class LightType : i32
{
	None = -1,
	Directional,
	Point,
	Spot,
};

// https://wiki.ogre3d.org/tiki-index.php?page=-Point+Light+Attenuation
static constexpr auto ATTENUATION_RANGES = {
	Array<f32, 3>{ 7, 0.7f, 1.8f },
	Array<f32, 3>{ 13, 0.35f, 0.44f },
	Array<f32, 3>{ 20, 0.22f, 0.20f },
	Array<f32, 3>{ 32, 0.14f, 0.07f },
	Array<f32, 3>{ 50, 0.09f, 0.032f },
	Array<f32, 3>{ 65, 0.07f, 0.017f },
	Array<f32, 3>{ 100, 0.045f, 0.0075f },
	Array<f32, 3>{ 160, 0.027f, 0.0028f },
	Array<f32, 3>{ 200, 0.022f, 0.0019f },
	Array<f32, 3>{ 325, 0.014f, 0.0007f },
	Array<f32, 3>{ 600, 0.007f, 0.0002f },
	Array<f32, 3>{ 3250, 0.0014f, 0.000007f },
};

namespace Components
{
	struct Light
	{
		LightType type;
	};

	/**
	 * @brief
	 * When a light source is modeled to be infinitely far away it is called a
	 * directional light since all its light rays have the same direction;
	 * it is independent of the location of the light source.
	 * A fine example of a directional light source is the sun as we know it.
	 * The sun is not infinitely far away from us, but it is so far away that we can
	 * perceive it as being infinitely far away in the lighting calculations
	 */
	struct DirectionalLight
	{
		Vec4F color{ 1.0f }; // Vec4F is needed for std140 alignment
		Vec4F direction{ 0.0f, -1.0f, 0.0f, 0.f };
		f32 intensity{ 1.0f };

		f32 __padding[3]; // Needed for std140 alignment
	};

	/**
	 * @brief
	 * A point light is a light source with a given position somewhere in a world
	 * that illuminates in all directions, where the light rays fade out over distance.
	 * Think of light bulbs and torches as light casters that act as a point light.
	 */
	struct PointLight
	{
		Vec4F color{ 1.0f };
		Vec4F position{ 0.0f, 1.0f, 0.0f, 0.f };
		f32 intensity{ 1.0f };
		// attenuation
		f32 kl{ 0.09f };	// Linear attenuation factor
		f32 kq{ 0.032f };	// Quadratic attenuation factor

		f32 __padding;
	};

	struct SpotLight
	{
		Vec4F direction{ 0.0f, -1.0f, 0.0f, 0.f };
		Vec4F position{ 0.0f, 1.0f, 0.0f, 0.f };
		Vec4F color{ 1.0f };
		f32 intensity{ 1.f };
		// attenuation
		f32 kl{ 0.09f };			// Linear attenuation factor
		f32 kq{ 0.032f };			// Quadratic attenuation factor
		// Shadow angle
		f32 thetaU{ 17.5f };	// Penumbral angle. This is the angle of the inner cone (in degrees).
		f32 thetaP{ 12.5f };	// Shadow angle. This is the angle of the outer cone (in degrees).

		f32 __padding[3];
	};
}



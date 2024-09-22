#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Math.hpp"

enum class LightType : i32
{
	DIRECTIONAL = 0,
	POINT = 1,
	SPOT = 2,
};

/* https://wiki.ogre3d.org/tiki-index.php?page=-Point+Light+Attenuation */
struct Attenuation
{
	i32 range{ 50 };	/* If an objects distance is greater than the range, the light has no effect on the object */
	f32 kl{ 0.09f };	/* Linear attenuation factor */
	f32 kq{ 0.032f }; /* Quadratic attenuation factor */
};

static constexpr const Attenuation ATTENUATION_RANGES[] = {
	Attenuation{ 7, 0.7f, 1.8f },
	Attenuation{ 13, 0.35f, 0.44f },
	Attenuation{ 20, 0.22f, 0.20f },
	Attenuation{ 32, 0.14f, 0.07f },
	Attenuation{ 50, 0.09f, 0.032f },
	Attenuation{ 65, 0.07f, 0.017f },
	Attenuation{ 100, 0.045f, 0.0075f },
	Attenuation{ 160, 0.027f, 0.0028f },
	Attenuation{ 200, 0.022f, 0.0019f },
	Attenuation{ 325, 0.014f, 0.0007f },
	Attenuation{ 600, 0.007f, 0.0002f },
	Attenuation{ 3250, 0.0014f, 0.000007f },
};

struct Light
{
	LightType type;
};

/**
 * When a light source is modeled to be infinitely far away it is called a
 * directional light since all its light rays have the same direction;
 * it is independent of the location of the light source.
 * A fine example of a directional light source is the sun as we know it.
 * The sun is not infinitely far away from us, but it is so far away that we can
 * perceive it as being infinitely far away in the lighting calculations
 */
struct DirectionalLight
{
	vec3f color{ 1.0f,1.0f,1.0f };
	f32 diffuseIntensity{ 0.5f };
	f32 specularIntensity{ 0.5f };

	vec3f direction{ 0.0f, -1.0f, 0.0f };
};


/**
 * A point light is a light source with a given position somewhere in a world
 * that illuminates in all directions, where the light rays fade out over distance.
 * Think of light bulbs and torches as light casters that act as a point light.
 */
struct PointLight
{
	vec3f color{ 1.0f,1.0f,1.0f };
	f32 diffuseIntensity{ 0.5f };
	f32 specularIntensity{ 0.5f };

	vec3f position{ 0.0f, 0.0f, 0.0f };

	Attenuation attenuation;
};

/**
 * A spotlight is a light source that is located somewhere in the environment
 * that,instead of shooting light rays in all directions, only shoots them in
 * a specific direction.
 * A good example of a spotlight would be a street lamp or a flashlight.
 */
struct SpotLight
{
	vec3f color{ 1.0f,1.0f,1.0f };
	f32 diffuseIntensity{ 0.5f };
	f32 specularIntensity{ 0.5f };

	vec3f direction{ 0.0f, -1.0f, 0.0f };
	vec3f position{ 0.0f, 0.0f, 0.0f };

	Attenuation attenuation;

	/* Spotlight's radius */
	f32 cutOff{ 12.5f };
	f32 outerCutOff{ 17.5f };
};
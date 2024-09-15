#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Math.hpp"

/* https://wiki.ogre3d.org/tiki-index.php?page=-Point+Light+Attenuation */
struct Attenuation
{
	f32 kl{ 0.09f };	/* Linear attenuation factor */
	f32 kq{ 0.032f }; /* Quadratic attenuation factor */
};

/**
 * When a light source is modeled to be infinitely far away it is called a
 * directional light since all its light rays have the same direction;
 * it is independent of the location of the light source.
 * A fine example of a directional light source is the sun as we know it.
 * The sun is not infinitely far away from us, but it is so far away that we can
 * perceive it as being infinitely far away in the lighting calculations
 */
class DirectionalLight
{
public:
	DirectionalLight() = default;
	~DirectionalLight() = default;

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
class PointLight
{
public:
	PointLight() = default;
	~PointLight() = default;

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
class SpotLight
{
public:
	SpotLight() = default;
	~SpotLight() = default;

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
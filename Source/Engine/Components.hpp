#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Math.hpp"
#include "Engine/Material.hpp"
#include "Engine/Graphics/VertexArray.hpp"


/**
 * Represents the GameObject's transformation (location, rotation, scale) in world space
 */
class TransformComponent
{
public:
	TransformComponent();
	~TransformComponent() = default;

	/**  
	 * Return following string representation:
	 * "position=<position.x,position.y,position.z>"
	 * "scale=<scale.x,scale.y,scale.z>"
	 * "rotation=<rotation.x,rotation.y,rotation.z>"
	 */
	//void Format(string& out) const;

	/**
	 * Update the transformation matrix. Call this function after changing position, scale or rotation.
	 */
	void UpdateTransformation();

	/**
	 * Calculate the model matrix
	 */
	Mat4f& GetTransformation();

	Vec3f position{ 0.0f, 0.0f, 0.0f };
	Vec3f scale{ 1.0f, 1.0f, 1.0f };
	Vec3f rotation{ 0.0f, 0.0f, 0.0f };	/* In degrees */

private:
	Mat4f _transformation;
};


/**
 * A Static Mesh is a piece of geometry that consists of a set of polygons that
 * can be cached in video memory and rendered by the graphics card.
 * This allows them to be rendered efficiently, meaning they can be much more
 * complex than other types of geometry such as Brushes.
 * Since they are cached in video memory, Static Meshes can be translated, rotated,
 * and scaled, but they cannot have their vertices animated in any way.
 */
class StaticMeshComponent
{
public:
	StaticMeshComponent(const fspath& filePath);
	~StaticMeshComponent() = default;

	void Draw();

	/**  
	 * Return following string representation:
	 * "model-path=<path>"													(if exists)
	 * "material-diffuse=<material.diffuse.path>"		(if exists)
	 * "material-specular=<material.specular.path>"	(if exists)
	 */
	//void Format(string& out) const;

	/**
	 * Free GPU memory
	 */
	void DestroyMesh();

	VertexArray vao;
	Material		material;
	fspath			modelPath;
};


/**
 * When a light source is modeled to be infinitely far away it is called a 
 * directional light since all its light rays have the same direction;
 * it is independent of the location of the light source.
 * A fine example of a directional light source is the sun as we know it.
 * The sun is not infinitely far away from us, but it is so far away that we can 
 * perceive it as being infinitely far away in the lighting calculations
 */
struct DirLightComponent
{
public:
	DirLightComponent() = default;
	~DirLightComponent() = default;

	void RenderLight(class Program& program) const;

	/**  
	 * Return following string representation:
	 * "color=<color.x,color.y,color.z>"
	 * "ambient=<ambient>"
	 * "diffuse=<diffuse>"
	 * "specular=<specular>"
	 * "direction=<direction.x,direction.y,direction.z>"
	 */
	//void Format(string& out) const;

	Vec3f color{ 0.0f,0.0f,0.0f };  /* light color */
	float ambient{ 0.0f };					/* light ambient intensity */
	float diffuse{ 0.0f };					/* light diffuse intensity */
	float specular{ 0.0f };					/* light specular intensity */

	Vec3f direction{ 0.0f, -1.0f, 0.0f };
};


/**
 * A point light is a light source with a given position somewhere in a world
 * that illuminates in all directions, where the light rays fade out over distance.
 * Think of light bulbs and torches as light casters that act as a point light.
 */
class PointLightComponent
{
public:
	PointLightComponent() = default;
	~PointLightComponent() = default;

	void RenderLight(class Program& program) const;

	/**  
	 * Return following string representation:
	 * "color=<color.x,color.y,color.z>"
	 * "ambient=<ambient>"
	 * "diffuse=<diffuse>"
	 * "specular=<specular>"
	 * "position=<position.x,position.y,position.z>"
	 */
	//void ToString(string& out) const override;

	Vec3f color{ 0.0f,0.0f,0.0f };  /* light color */
	float ambient{ 0.0f };					/* light ambient intensity */
	float diffuse{ 0.0f };					/* light diffuse intensity */
	float specular{ 0.0f };					/* light specular intensity */

	Vec3f position{ 0.0f, 0.0f, 0.0f };

	/* attenuation */
	/* https://wiki.ogre3d.org/tiki-index.php?page=-Point+Light+Attenuation */
	float linear{ 0.09f };
	float quadratic{ 0.032f };
};

/**
 * A spotlight is a light source that is located somewhere in the environment 
 * that,instead of shooting light rays in all directions, only shoots them in 
 * a specific direction.
 * A good example of a spotlight would be a street lamp or a flashlight.
 */
class SpotLightComponent
{
public:
	SpotLightComponent() = default;
	~SpotLightComponent() = default;

	void RenderLight(class Program& program) const;

	/** 
	 * Return following string representation:
	 * "color=<color.x,color.y,color.z>"
	 * "ambient=<ambient>"
	 * "diffuse=<diffuse>"
	 * "direction=<direction.x,direction.y,direction.z>"
	 * "cutoff=<cutoff>"
	 */
	//void ToString(string& out) const override;
		
	Vec3f color{ 0.0f,0.0f,0.0f };  /* light color */
	float ambient{ 0.0f };					/* light ambient intensity */
	float diffuse{ 0.0f };					/* light diffuse intensity */
	float specular{ 0.0f };					/* light specular intensity */

	Vec3f direction{ 0.0f, -1.0f, 0.0f };
	Vec3f position{ 0.0f, 0.0f, 0.0f };
	
	/* attenuation */
	/* https://wiki.ogre3d.org/tiki-index.php?page=-Point+Light+Attenuation */
	float linear{ 0.09f };
	float quadratic{ 0.032f };

	/* spotlight's radius */
	float cutOff{ Math::Cos(Math::Radians(12.5)) }; 
	float outerCutOff{ Math::Cos(Math::Radians(17.5)) };
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
class CameraComponent
{
public:
	CameraComponent(
		const Vec3f& position = { 0.0f, 0.0f, 0.0f }, /* default position*/
		float fov = 45.0f,					/* default field of view */
		float aspect = 16.0f/9.0f		/* default aspect ratio 16:9 */
	);
	~CameraComponent() = default;

	/** 
	 * Return following string representation:
	 * "position=<position.x,position.y,position.z>"
	 * "yaw=<yaw>"
	 * "pitch=<pitch>"
	 * "roll=<roll>"
	 * "fov=<fov>"
	 * "aspect=<aspect>"
	 * "zNear=<zNear>"
	 * "zFar=<zFar>"
	 */
	//void Format(string& out) const;

	const Mat4f& GetView() { return _viewMatrix; }
	const Mat4f& GetProjection() { return _projectionMatrix; }

	const Vec3f& GetFrontVector() { return _front; }
	const Vec3f& GetRightVector() { return _right; }
	const Vec3f& GetUpVector() { return _up; }
	
	/**
	 * Update Orientation vectors
	 */
	void UpdateVectors();
	
	/**
	 * Update the view matrix. Call this function every frame.
	 */
	void UpdateView();
	
	/**
	 * Update the projection matrix. Call this function after changing fov, aspect, znear and zfar
	 */
	void UpdateProjection();

	Vec3f position;

	/* Euler angles (in degrees) */
	float yaw;
	float pitch;
	float roll;

	float fov;
	float aspect;
	float zNear;
	float zFar;

private:
	Mat4f _viewMatrix;
	Mat4f _projectionMatrix;

	/* Orientation vectors */
	Vec3f _front;
	Vec3f _up;
	Vec3f _right;
};
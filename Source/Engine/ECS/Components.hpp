#pragma once

#include "Engine/ECS/IComponent.hpp"
#include "Core/Math/Math.hpp"

#include "Engine/Material.hpp"

/* --------------------------------------------------------------------------- 
	TypeComponent: 
	represents a type of a GameObject
	--------------------------------------------------------------------------- */
struct TypeComponent : public IComponent
{
	TypeComponent(uint32_t type);

	static const char* GetComponentName(bool lower = false);

	/* Return "type=<type>" */
	void ToString(String& out) const override;

	uint32_t type;
};


/* ---------------------------------------------------------------------------
	LabelComponent:
	represents a label of a GameObject
	--------------------------------------------------------------------------- */
struct LabelComponent : public IComponent
{
	LabelComponent(const char* label);
	
	static const char* GetComponentName(bool lower = false);

	/* Return string "label=<label>" */
	void ToString(String& out) const override;

	String label;
};


/* ---------------------------------------------------------------------------
	TransformComponent: 
	represents the transformation (location, rotation, scale) of a GameObject   
	--------------------------------------------------------------------------- */
struct TransformComponent : public IComponent
{
	TransformComponent();

	static const char* GetComponentName(bool lower = false);

	/* Return following string representation:
		"position=<position.x,position.y,position.z>"
		"scale=<scale.x,scale.y,scale.z>"
		"rotation=<rotation.x,rotation.y,rotation.z>"
	*/
	void ToString(String& out) const override;

	/* Calculate the model matrix */
	Mat4f GetTransformation() const;

	Vec3f position;
	Vec3f scale;
	Vec3f rotation;	/* in degrees */
};


/* ---------------------------------------------------------------------------
	A Static Mesh is a piece of geometry that consists of a set of polygons that
	can be cached in video memory and rendered by the graphics card.
	This allows them to be rendered efficiently, meaning they can be much more
	complex than other types of geometry such as Brushes.
	Since they are cached in video memory, Static Meshes can be translated, rotated,
	and scaled, but they cannot have their vertices animated in any way.
	--------------------------------------------------------------------------- */
struct StaticMeshComponent : public IComponent
{
	/* Create empty vertex array object. You Must call InitMesh to initialize buffers */
	StaticMeshComponent();

	/* Initialize Mesh component from given data */
	StaticMeshComponent(const class VertexBufferLayout& layout, const class VertexBufferData& data);

	/* Initialize Mesh getting data from .obj file */
	StaticMeshComponent(const Path& objFilePath);

	/* The destructor do nothing. You must call DestroyMesh to destroy vertex array and free memory */
	~StaticMeshComponent() = default;

	static const char* GetComponentName(bool lower = false);

	/* Return following string representation:
		"model-path=<path>"														if exists
		"material-diffuse=<material.diffuse.path>"		if exists
		"material-specular=<material.specular.path>"	if exists
	*/
	void ToString(String& out) const override;

	/* Initialize buffers with given data */
	void InitMesh(const VertexBufferLayout& layout, const VertexBufferData& data) const;

	/* Destroy vertex array */
	void DestroyMesh() const;

	/* Draw mesh */
	//void Draw(const Mat4f& transform, class Shader* shader) const;

	/* By default it does not contain textures */
	Material material;

	Path modelPath;

	//SharedPointer<VertexArray> vertexArray;
	class VertexArray* vertexArray;
};


/* ---------------------------------------------------------------------------
	LightComponent:
	contains base light attributes such as color, ambient, diffuse, specular.
	DirectionalLight, PointLight and SpotLight inherit from this class
	--------------------------------------------------------------------------- */
struct LightComponent : public IComponent
{
	LightComponent(const char* uniform);

	/* Return following string representation:
		"color=<color.x,color.y,color.z>"
		"ambient=<ambient>"
		"diffuse=<diffuse>"
		"specular=<specular>"
	*/
	void ToString(String& out) const override;

	/* Set shader uniform attributes */
	virtual void Render(class Shader& shader) {}

	Vec3f color;    /* light color */
	float ambient;  /* ambient intensity */
	float diffuse;  /* diffuse intensity */
	float specular; /* specular intensity */

	String uniform;	/* Used in shader */
};


/* ---------------------------------------------------------------------------
	When a light source is modeled to be infinitely far away it is called a 
	directional light since all its light rays have the same direction;
	it is independent of the location of the light source.
	A fine example of a directional light source is the sun as we know it.
	The sun is not infinitely far away from us, but it is so far away that we can 
	perceive it as being infinitely far away in the lighting calculations
	--------------------------------------------------------------------------- */
struct DirLightComponent : public LightComponent
{
	DirLightComponent(const char* uniform);

	static const char* GetComponentName(bool lower = false);

	/* Return following string representation:
		"color=<color.x,color.y,color.z>"
		"ambient=<ambient>"
		"diffuse=<diffuse>"
		"specular=<specular>"
		"direction=<direction.x,direction.y,direction.z>"
	*/
	void ToString(String& out) const override;

	void Render(class Shader& shader) override;

	Vec3f direction;
};


/* ---------------------------------------------------------------------------
	A point light is a light source with a given position somewhere in a world
	that illuminates in all directions, where the light rays fade out over distance.
	Think of light bulbs and torches as light casters that act as a point light.
	--------------------------------------------------------------------------- */
struct PointLightComponent : public LightComponent
{
	PointLightComponent(const char* uniform);

	static const char* GetComponentName(bool lower = false);

	/* Return following string representation:
		"color=<color.x,color.y,color.z>"
		"ambient=<ambient>"
		"diffuse=<diffuse>"
		"specular=<specular>"
		"position=<position.x,position.y,position.z>"
	*/
	void ToString(String& out) const override;
	
	void Render(class Shader& shader) override;

	Vec3f position;

	/* Attenuation */
	float linear;
	float quadratic;
};

/* ---------------------------------------------------------------------------
	A spotlight is a light source that is located somewhere in the environment 
	that,instead of shooting light rays in all directions, only shoots them in 
	a specific direction.
	A good example of a spotlight would be a street lamp or a flashlight.
	--------------------------------------------------------------------------- */
class SpotLightComponent : PointLightComponent
{
public:
	SpotLightComponent(const char* uniform);

	static const char* GetComponentName(bool lower = false);

	/* Return following string representation:
		"color=<color.x,color.y,color.z>"
		"ambient=<ambient>"
		"diffuse=<diffuse>"
		"direction=<direction.x,direction.y,direction.z>"
		"cutoff=<cutoff>"
	*/
	void ToString(String& out) const override;

	void Render(class Shader& shader) override;

	Vec3f direction;

	float cutOff; /* the cutoff angle that specifies the spotlight's radius. */
};



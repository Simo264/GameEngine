#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Math.hpp"
#include "Engine/Material.hpp"
#include "Engine/Graphics/VertexArray.hpp"

/*
	Component list:
		-TypeComponent	(is it really useful?)
		-LabelComponent
		-TransformComponent
		-StaticMeshComponent
		-LightComponent
		-DirLightComponent
		-PointLightComponent
		-SpotLightComponent
		-CameraComponent
*/

/* ---------------------------------------------------------------------------
	Component Interface
	--------------------------------------------------------------------------- */
struct IComponent
{
	IComponent() = default;
	~IComponent() = default;

	/* For ech attribute in component return the following string:
		"<attr1>=<value1>"
		"<attr2>=<value2>"
		...
	*/
	virtual void ToString(String& out) const = 0;
};


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

	/* Update the transformation matrix. Call this function after changing position, scale or rotation. */
	void UpdateTransformation();

	/* Calculate the model matrix */
	Mat4f& GetTransformation();

	Vec3f position;
	Vec3f scale;
	Vec3f rotation;	/* In degrees */

private:
	Mat4f _transformation;
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

	void RenderMesh(const Mat4f& transform, class Shader& shader) const;

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

	SharedPointer<VertexArray> vertexArray;
};


/* ---------------------------------------------------------------------------
	LightComponent:
	contains base light attributes such as color, ambient, diffuse, specular.
	DirectionalLight, PointLight and SpotLight inherit from this class
	--------------------------------------------------------------------------- */
struct LightComponent : public IComponent
{
	LightComponent(const char* uniform);

	virtual void RenderLight(class Shader& shader) = 0;

	/* Return following string representation:
		"color=<color.x,color.y,color.z>"
		"ambient=<ambient>"
		"diffuse=<diffuse>"
		"specular=<specular>"
	*/
	void ToString(String& out) const override;

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

	void RenderLight(class Shader& shader) override;

	/* Return following string representation:
		"color=<color.x,color.y,color.z>"
		"ambient=<ambient>"
		"diffuse=<diffuse>"
		"specular=<specular>"
		"direction=<direction.x,direction.y,direction.z>"
	*/
	void ToString(String& out) const override;

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

	void RenderLight(class Shader& shader) override;

	/* Return following string representation:
		"color=<color.x,color.y,color.z>"
		"ambient=<ambient>"
		"diffuse=<diffuse>"
		"specular=<specular>"
		"position=<position.x,position.y,position.z>"
	*/
	void ToString(String& out) const override;

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
struct SpotLightComponent : PointLightComponent
{
	SpotLightComponent(const char* uniform);

	static const char* GetComponentName(bool lower = false);

	void RenderLight(class Shader& shader) override;

	/* Return following string representation:
		"color=<color.x,color.y,color.z>"
		"ambient=<ambient>"
		"diffuse=<diffuse>"
		"direction=<direction.x,direction.y,direction.z>"
		"cutoff=<cutoff>"
	*/
	void ToString(String& out) const override;
		
	Vec3f direction;

	float cutOff; /* the cutoff angle that specifies the spotlight's radius. */
};


/*	---------------------------------------------------------------------------
	When we're talking about camera/view space we're talking about all the vertex 
	coordinates as seen from the camera's perspective as the origin of the scene: 
	the view matrix transforms all the world coordinates into view coordinates 
	that are relative to the camera's position and direction. 
	To define a camera we need its position in world space, the direction 
	it's looking at, a vector pointing to the right and a vector pointing upwards 
	from the camera. 
	A careful reader may notice that we're actually going to create a coordinate 
	system with 3 perpendicular unit axes with the camera's position as the origin.
	--------------------------------------------------------------------------- */
struct CameraComponent : public IComponent
{
	CameraComponent(
		const Vec3f& position = { 0.0f, 0.0f, 0.0f }, /* default position*/
		float fov = 45.0f,					/* default field of view */
		float aspect = 16.0f/9.0f		/* default aspect ratio 16:9 */
	);

	/* Return following string representation:
		"position=<position.x,position.y,position.z>"
		"yaw=<yaw>"
		"pitch=<pitch>"
		"roll=<roll>"
		"fov=<fov>"
		"aspect=<aspect>"
		"zNear=<zNear>"
		"zFar=<zFar>"
	*/
	void ToString(String& out) const override;

	Mat4f& GetView();
	Mat4f& GetProjection();

	Vec3f& GetFrontVector();
	Vec3f& GetRightVector();
	Vec3f& GetUpVector();
	
	/* Update Orientation vectors */
	void UpdateVectors();
	/* Update the view matrix. Call this function every frame */
	void UpdateView();
	/* Update the projection matrix. Call this function after changing fov, aspect, znear and zfar */
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

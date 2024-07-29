#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Math.hpp"
#include "Engine/Material.hpp"
#include "Engine/Graphics/VertexArray.hpp"

class Texture2D;

struct aiNode;
struct aiScene;
struct aiMesh;
struct aiMaterial;
enum aiTextureType;

namespace Components
{
	class Label
	{
	public:
		Label(const char* label) : label{label} {}

		string label;
	};

	/**
	 * Represents the GameObject's transformation (location, rotation, scale) in world space
	 */
	class Transform
	{
	public:
		Transform();
		~Transform() = default;

		/**
		 * Update the transformation matrix. Call this function after changing position, scale or rotation.
		 */
		void UpdateTransformation();

		/**
		 * Calculate the model matrix
		 */
		mat4f& GetTransformation() { return _transformation;  }

		vec3f position{ 0.0f, 0.0f, 0.0f };
		vec3f scale{ 1.0f, 1.0f, 1.0f };
		vec3f rotation{ 0.0f, 0.0f, 0.0f };	/* In degrees */

	private:
		mat4f _transformation;
	};


	/**
	 * A mesh typically refers to a collection of vertices, edges, and faces that define the shape of a 3D object.
	 * A mesh is a fundamental concept in 3D computer graphics and is used to represent the geometry of objects
	 * that are rendered in a scene.
	 * Is composed of vertices, which are points in 3D space, connected by edges, which are line segments
	 * that define the boundaries of the mesh, and faces, which are polygons formed by
	 * connecting three or more vertices. These elements collectively define the shape and structure of the object
	 * being rendered.
	 * Meshes are often represented using vertex buffer objects (VBOs) and index buffer objects (IBOs) to efficiently
	 * store and manipulate the vertex data on the GPU.
	 */
	class Mesh
	{
	public:
		Mesh();
		Mesh(void* vertices, uint32_t numVertices, void* indices, uint32_t numIndices);

		~Mesh() = default;

		VertexArray vao;
		Buffer vbo;
		Buffer ebo;
		Material material;

		void DestroyMesh();
		void DrawMesh(int mode);

	private:
		void Init();
	};


	/**
	 * A model contains multiple meshes, possibly with multiple textures.
	 * A house, that contains a wooden balcony, a tower, and perhaps a swimming pool, could still be loaded as a single model.
	 * We'll load the model via Assimp and translate it to multiple Mesh objects.
	 */
	class Model
	{
	public:
		Model(const fspath& path);

		Mesh* meshes;
		uint32_t numMeshes;

		fspath modelPath;

		void DestroyModel();
		void DrawModel(int mode);

	private:
		void ProcessNode(aiNode* node, const aiScene* scene);
		void LoadVertices(aiMesh* aimesh, Buffer& vbo);
		void LoadIndices(aiMesh* aimesh, Buffer& ebo);
		Texture2D* GetTexture(aiMaterial* material, aiTextureType type);

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
		float diffuseIntensity{ 0.5f };		/* range [0-1] */
		float specularIntensity{ 0.5f };	/* range [0-1] */
	
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
		float diffuseIntensity{ 0.5f };		/* range [0-1] */
		float specularIntensity{ 0.5f };	/* range [0-1] */

		vec3f position{ 0.0f, 0.0f, 0.0f };

		// https://wiki.ogre3d.org/tiki-index.php?page=-Point+Light+Attenuation
		struct Attenuation 
		{
			float kl{ 0.09f };	/* linear attenuation factor */
			float kq{ 0.032f }; /* quadratic attenuation factor */
		} attenuation;
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
		float diffuseIntensity{ 0.5f };		/* range [0-1] */
		float specularIntensity{ 0.5f };	/* range [0-1] */

		vec3f direction{ 0.0f, -1.0f, 0.0f };
		vec3f position{ 0.0f, 0.0f, 0.0f };
	
		// https://wiki.ogre3d.org/tiki-index.php?page=-Point+Light+Attenuation 
		struct Attenuation
		{
			float kl{ 0.09f };	/* linear attenuation factor */
			float kq{ 0.032f }; /* quadratic attenuation factor */
		} attenuation;

		/* spotlight's radius */
		float cutOff{ 12.5f };
		float outerCutOff{ 17.5f };
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
	class Camera
	{
	public:
		Camera(
			const vec3f& position = { 0.0f, 0.0f, 0.0f },
			float fov = 45.0f,		
			float aspect = 16.0f/9.0f,
			float znear = 1.0f,
			float zfar = 10.0f
		);
		~Camera() = default;

		const mat4f& GetView() { return _viewMatrix; }
		const mat4f& GetProjection() { return _projectionMatrix; }

		const vec3f& GetFrontVector() { return _front; }
		const vec3f& GetRightVector() { return _right; }
		const vec3f& GetUpVector() { return _up; }
	
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

		vec3f position;

		/* Euler angles (in degrees) */
		float yaw;
		float pitch;
		float roll;

		float fov;
		float aspect;
		float zNear;
		float zFar;

	private:
		mat4f _viewMatrix;
		mat4f _projectionMatrix;

		/* Orientation vectors */
		vec3f _front;
		vec3f _up;
		vec3f _right;
	};
};
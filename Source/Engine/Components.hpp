#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Math.hpp"
#include "Engine/Material.hpp"
#include "Engine/Graphics/Containers/VertexArray.hpp"

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
		Label(const char* label) : value{ label } {}
		~Label() = default;

		String value;
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
		~Mesh() = default;

		VertexArray vao;
		Material material;

		void DestroyMesh();
		void DrawMesh(i32 mode);

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
		Model(const fs::path& path);
		~Model() = default;

		Vector<Mesh> meshes;

		fs::path modelPath;

		void DestroyModel();
		void DrawModel(i32 mode);

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
		f32 diffuseIntensity{ 0.5f };		/* range [0-1] */
		f32 specularIntensity{ 0.5f };	/* range [0-1] */
	
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
		f32 diffuseIntensity{ 0.5f };		/* range [0-1] */
		f32 specularIntensity{ 0.5f };	/* range [0-1] */

		vec3f position{ 0.0f, 0.0f, 0.0f };

		// https://wiki.ogre3d.org/tiki-index.php?page=-Point+Light+Attenuation
		struct Attenuation 
		{
			f32 kl{ 0.09f };	/* linear attenuation factor */
			f32 kq{ 0.032f }; /* quadratic attenuation factor */
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
		f32 diffuseIntensity{ 0.5f };		/* range [0-1] */
		f32 specularIntensity{ 0.5f };	/* range [0-1] */

		vec3f direction{ 0.0f, -1.0f, 0.0f };
		vec3f position{ 0.0f, 0.0f, 0.0f };
	
		// https://wiki.ogre3d.org/tiki-index.php?page=-Point+Light+Attenuation 
		struct Attenuation
		{
			f32 kl{ 0.09f };	/* linear attenuation factor */
			f32 kq{ 0.032f }; /* quadratic attenuation factor */
		} attenuation;

		/* spotlight's radius */
		f32 cutOff{ 12.5f };
		f32 outerCutOff{ 17.5f };
	};
};
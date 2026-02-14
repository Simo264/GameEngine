#pragma once

#include "Core/Types.hpp"
#include "Core/FileSystem.hpp"

namespace Components
{
	struct Material;
	struct StaticMesh;
}
struct Buffer;
struct aiMesh;

class StaticMeshLoader
{
public:
	StaticMeshLoader() = default;

	/**
	 * @brief Loads mesh data from a 3D model file using Assimp library.
	 * This method reads a 3D model file (such as .obj, .fbx, .dae, etc.) and populates
	 * the provided StaticMesh with vertex and index data. It creates immutable storage
	 * buffers and writes the loaded geometry data directly into GPU memory.
	 *
	 * @param absolutePathToFile Absolute filesystem path to the 3D model file
	 * @param meshDest The destination StaticMesh object that will be populated with loaded data
	 *
	 * @pre The destination StaticMesh must be in a valid state (Create() must have been called)
	 * @pre The destination StaticMesh must have its vertex layout already configured
	 *
	 * @note This method does NOT call Create() on the destination mesh
	 * @note This method does NOT configure the vertex attribute layout
	 * @note The vertex array object and vertex layout must be set up separately
	 *
	 * @warning The method assumes triangulated geometry (aiProcess_Triangulate flag is used)
	 * @warning Calling this method on an invalid destination mesh may result in undefined behavior
	 * @warning If the file contains no mesh data, the operation will fail
	 *
	 * @see StaticMesh::Create()
	 * @see VertexLayout::SetupVertexArray()
	 *
	 * @example
	 * StaticMesh mesh;
	 * mesh.Create();  // Initialize OpenGL objects
	 *
	 * using Vertex = VertexLayout<Position, Normal, TextureCoord, Tangent>;
	 * Vertex::SetupVertexArray(mesh.vertexArray);  // Configure layout
	 *
	 * StaticMeshLoader loader;
	 * bool success = loader.LoadDataFromFile("models/cube.obj", mesh);
	 */
	void LoadDataFromFile(const fs::path& absolutePathToFile, 
												Components::StaticMesh& meshDest,
												Components::Material& materialDest);

private:
	template<typename Vertex>
	void __LoadVertices(Buffer& vertBuffer, u32 nrVertices, aiMesh* aimesh);
	void __LoadIndices(Buffer& indexBuffer, aiMesh* aimesh);
};
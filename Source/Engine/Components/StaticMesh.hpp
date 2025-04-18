#pragma once

#include "Core/Core.hpp"
#include "Engine/Mesh.hpp"

class StaticMesh
{
public:
	StaticMesh() : 
		prototypeID{ -1 },
		meshArray{},
		nrMeshes{ 0 }
	{}
	~StaticMesh() = default;

	/** @brief Move constructor */
	StaticMesh(StaticMesh&&) noexcept = default;
	StaticMesh& operator=(StaticMesh&&) noexcept = default;

	/** @brief Destroys all meshes associated with the static mesh. */
	void Destroy();

	/** @brief Renders all meshes associated with the static mesh. */
	void Render(class Program program, RenderMode mode) const;

	/** @brief Creates a copy of the current StaticMesh into another StaticMesh instance. */
	void Copy(StaticMesh& other) const;

	/** @brief List of meshes contained in the model. */
	UniquePointer<Mesh[]> meshArray;
	u32 nrMeshes;

	/** @brief Unique identifier for the prototype of the static mesh. */
	i32 prototypeID;
};

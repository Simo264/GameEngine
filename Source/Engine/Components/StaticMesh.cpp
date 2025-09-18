#include "StaticMesh.hpp"
#include "Engine/Graphics/Shader.hpp"

namespace Components
{
	void StaticMesh::Destroy()
	{
		for (auto i = 0u; i < nrMeshes; i++)
			meshArray[i].Destroy();

		meshArray.reset();
	}
	
	void StaticMesh::Render(Program program, RenderMode mode) const
	{
		for (auto i = 0u; i < nrMeshes; i++)
			meshArray[i].Render(program, mode);
	}

	void StaticMesh::Copy(StaticMesh& other) const
	{
		other.prototypeID = prototypeID;
		other.nrMeshes = nrMeshes;
		other.meshArray = std::make_unique<Mesh[]>(nrMeshes);
		for (auto i = 0u; i < nrMeshes; i++)
			meshArray[i].Share(other.meshArray[i]);
	}
}



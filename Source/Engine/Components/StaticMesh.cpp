#include "StaticMesh.hpp"


#include "Engine/Graphics/Shader.hpp"

// ----------------------------------------------------
//										PUBLIC
// ----------------------------------------------------

void StaticMesh::Destroy()
{
	for (u32 i = 0; i < nrMeshes; i++)
		meshArray[i].Destroy();
	
	meshArray.reset();
}
void StaticMesh::Render(Program program, RenderMode mode) const
{
	for (u32 i = 0; i < nrMeshes; i++)
		meshArray[i].Render(program, mode);
}
void StaticMesh::Copy(StaticMesh& other) const
{
	other.prototypeID = prototypeID;
	other.nrMeshes = nrMeshes;
	other.meshArray = std::make_unique<Mesh[]>(nrMeshes);
	for (u32 i = 0; i < nrMeshes; i++)
		meshArray[i].Share(other.meshArray[i]);
}

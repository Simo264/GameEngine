#pragma once

#include "Core.hh"
#include "NonCopyable.hh"
#include "SceneObject.hh"
#include "Mesh/StaticMesh.hh"
#include "Lighting/DirectionalLight.hh"
#include "Lighting/PointLight.hh"

/* ----------------------------------------------------------------------------
	Scene class contains all of objects that are placed in world.	
	---------------------------------------------------------------------------- */
class Scene : public NonCopyable
{
public:
	Scene() : sceneDLight{ nullptr } {}

	void DrawScene(class Shader* shader);
	
	template<class T>
	void AddSceneObject(const SharedPointer<T>&);

	template<class T>
	void RemoveSceneObject(const SharedPointer<T>&);

	const bool HasDirLight() const { return sceneDLight.get(); };
	const bool HasPointLights() const { return !scenePLights.empty(); };
	const bool HasStaticMeshes() const { return !sceneSMeshes.empty(); };
	const bool IsEmpty() const { return !HasDirLight() && !HasPointLights() && !HasStaticMeshes(); }

	void ClearScene();

	/* Example of scene file:
	[DirectionalLight]
	uniformname="UDirLight"
	color=0,0,0
	ambient=0,0,0
	...

	[PointLight]
	uniformname="UPointLight[0]"
	color=0,0,0
	ambient=0,0,0
	...

	[StaticMesh]
	modelpath="..."
	*/
	//void LoadScene(Path filePath);

	/* Can be there only ONE directional light in scene */
	SharedPointer<DirectionalLight> sceneDLight;
	
	/* Can be there multiple point light in scene */
	Vector<SharedPointer<PointLight>> scenePLights;

	/* Can be there multiple static mesh in scene */
	Vector<SharedPointer<StaticMesh>> sceneSMeshes;
};

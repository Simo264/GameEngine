#pragma once

#include "NonCopyable.hh"
#include "SceneObject.hh"
#include "Mesh/StaticMesh.hh"
#include "Mesh/InstancingMesh.hh"
#include "Lighting/DirectionalLight.hh"
#include "Lighting/PointLight.hh"
#include "Lighting/SpotLight.hh"

/* ----------------------------------------------------------------------------
	Scene class represents the set of objects placed in world.	
	This class does not create/destroy objects in/from memory but only uses the 
	addresses of objects already allocated in memory 
	---------------------------------------------------------------------------- */
class Scene : public NonCopyable
{
public:
	Scene() : sceneDLight{ nullptr } {}
	~Scene() = default;

	void DrawScene(class Shader* shader);
	
	void AddDirectionalLight(SceneObject<DirectionalLight>& dirLight);
	void AddPointLight(SceneObject<PointLight>& pointLight);
	void AddStaticMesh(SceneObject<StaticMesh>& staticMesh);

	void RemoveDirectionalLight();
	void RemovePointLight(SceneObject<PointLight>& pointLight);
	void RemoveStaticMesh(SceneObject<StaticMesh>& staticMesh);

	const bool HasDirLight() const { return sceneDLight && sceneDLight->object; };
	const bool HasPointLights() const { return !scenePLights.empty(); };
	const bool HasStaticMeshes() const { return !sceneSMeshes.empty(); };

	const bool IsEmpty() const { return !HasDirLight() && !HasPointLights() && !HasStaticMeshes(); }

	/* Can be there only ONE directional light in scene */
	SceneObject<DirectionalLight>* sceneDLight;

	/* Can be there multiple point light in scene */
	Vector<SceneObject<PointLight>*> scenePLights;

	/* Can be there multiple static mesh in scene */
	Vector<SceneObject<StaticMesh>*> sceneSMeshes;
};

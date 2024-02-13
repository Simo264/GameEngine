#pragma once

#include "NonCopyable.hh"
#include "SceneObject.hh"
#include "Mesh/StaticMesh.hh"
#include "Mesh/InstancingMesh.hh"
#include "Lighting/DirectionalLight.hh"
#include "Lighting/PointLight.hh"
#include "Lighting/SpotLight.hh"

/* ----------------------------------------------------------------------------
	Scene class contains all of objects that are placed in world.	
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

	void ClearScene();
	void LoadScene(Path filePath);

	/* Can be there only ONE directional light in scene */
	SceneObject<DirectionalLight>* sceneDLight;

	/* Can be there multiple point light in scene */
	Vector<SceneObject<PointLight>*> scenePLights;

	/* Can be there multiple static mesh in scene */
	Vector<SceneObject<StaticMesh>*> sceneSMeshes;
};

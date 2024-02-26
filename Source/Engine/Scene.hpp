#pragma once

#include "Core/Core.hpp"

#include "Engine/StaticMesh.hpp"
#include "Engine/Lighting/DirectionalLight.hpp"
#include "Engine/Lighting/PointLight.hpp"

/* ----------------------------------------------------------------------------
	Scene class contains all of objects that are placed in world.	
	---------------------------------------------------------------------------- */
class Scene
{
public:
	Scene() : sceneDLight{ nullptr } {}

	void DrawScene(class Shader* shader);
	
	void AddSceneObject(const SharedPointer<DirectionalLight>&);
	void AddSceneObject(const SharedPointer<PointLight>&);
	void AddSceneObject(const SharedPointer<StaticMesh>&);

	void RemoveSceneObject(const SharedPointer<DirectionalLight>&);
	void RemoveSceneObject(const SharedPointer<PointLight>&);
	void RemoveSceneObject(const SharedPointer<StaticMesh>&);

	const bool HasDirLight() const { return sceneDLight.get(); };
	const bool HasPointLights() const { return !scenePLights.empty(); };
	const bool HasStaticMeshes() const { return !sceneSMeshes.empty(); };
	const bool IsEmpty() const { return !HasDirLight() && !HasPointLights() && !HasStaticMeshes(); }

	void ClearScene();

	void LoadScene(Path filepath);
	void SaveScene(Path outfile);

	/* Can be there only ONE directional light in scene */
	SharedPointer<DirectionalLight> sceneDLight;
	
	/* Can be there multiple point light in scene */
	Vector<SharedPointer<PointLight>> scenePLights;

	/* Can be there multiple static mesh in scene */
	Vector<SharedPointer<StaticMesh>> sceneSMeshes;

private:
	SharedPointer<DirectionalLight> ParseDirectionalLight(
		IFStream& file, String& line, char name[64], char value[64]);
	
	SharedPointer<PointLight> ParsePointLight(
		IFStream& file, String& line, char name[64], char value[64]);
	
	//SharedPointer<SpotLight> ParseDirectionalLight();
	
	SharedPointer<StaticMesh> ParseStaticMesh(
		IFStream& file, String& line, char name[64], char value[64]);
};

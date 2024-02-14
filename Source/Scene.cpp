#include "Scene.hh"
#include "Shader.hh"
#include "Logger.hh"

#include "SceneObject.hh"
#include "Mesh/StaticMesh.hh"
#include "Lighting/DirectionalLight.hh"
#include "Lighting/PointLight.hh"
#include "Lighting/SpotLight.hh"

/* -----------------------------------------------------
 *          PUBLIC METHODS
 * -----------------------------------------------------
*/

#if 0
void Scene::ClearScene() { /* TODO */ }

void Scene::LoadScene(Path filePath)
{
	/* Clean previuos scene */
	ClearScene();

	IFStream file(filePath.string());
	if (!file.is_open())
	{
		CONSOLE_WARN("Error on opening file '{}'", filePath.string());
		return;
	}
}
#endif

void Scene::DrawScene(Shader* shader)
{
	/* Render directional light */
	if (HasDirLight())
		if (sceneDLight->visible)
			static_cast<DirectionalLight*>(sceneDLight)->RenderLight(shader);

	/* Render point lights */
	for (auto scenePLight : scenePLights)
		if (scenePLight->visible)
			static_cast<PointLight*>(scenePLight)->RenderLight(shader);
	
	/* Render static mesh objects (enable cull face to improve performance) */
	for (auto sceneSMesh : sceneSMeshes)
		if (sceneSMesh->visible)
			static_cast<StaticMesh*>(sceneSMesh)->Draw(shader);
}

/* Add directional light in scene */
template<>
void Scene::AddSceneObject(DirectionalLight* dLight)
{
	sceneDLight = dLight;
}

/* Add point light in scene */
template<>
void Scene::AddSceneObject(PointLight* pLight)
{
	scenePLights.push_back(pLight);
}

/* Add static mesh in scene */
template<>
void Scene::AddSceneObject(StaticMesh* sMesh)
{
	sceneSMeshes.push_back(sMesh);
}


/* Remove directional light from scene */
template<>
void Scene::RemoveSceneObject(DirectionalLight*)
{
	this->sceneDLight = nullptr;
}

/* Remove point light from scene */
template<>
void Scene::RemoveSceneObject(PointLight* pLight)
{
	auto beg = scenePLights.begin();
	auto end = scenePLights.end();
	auto it = std::find_if(beg, end, [&](SceneObject* o) {
		return o->Compare(*pLight);
		});

	if (it != end)
		scenePLights.erase(it);
}

/* Remove static mesh from scene */
template<>
void Scene::RemoveSceneObject(StaticMesh* sMesh)
{
	auto beg = sceneSMeshes.begin();
	auto end = sceneSMeshes.end();
	auto it = std::find_if(beg, end, [&](SceneObject* o) {
		return o->Compare(*sMesh);
		});

	if (it != end)
		sceneSMeshes.erase(it);
}

/* -----------------------------------------------------
 *          PRIVATE METHODS
 * -----------------------------------------------------
*/




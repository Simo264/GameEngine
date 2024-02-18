#include "Scene.hh"
#include "Shader.hh"
#include "Logger.hh"


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

void Scene::ClearScene() 
{ 
	sceneDLight.reset();

	for (auto& pLight : scenePLights)
		pLight.reset();
	scenePLights.clear();

	for (auto& sMesh: sceneSMeshes)
		sMesh.reset();
	sceneSMeshes.clear();
}

template<>
void Scene::AddSceneObject(const SharedPointer<DirectionalLight>& obj)
{
	sceneDLight = obj;
}

template<>
void Scene::AddSceneObject(const SharedPointer<PointLight>& obj)
{
	scenePLights.push_back(obj);
}

template<>
void Scene::AddSceneObject(const SharedPointer<StaticMesh>& obj)
{
	sceneSMeshes.push_back(obj);
}

template<>
void Scene::RemoveSceneObject(const SharedPointer<DirectionalLight>& obj)
{
	sceneDLight = nullptr;
}

template<>
void Scene::RemoveSceneObject(const SharedPointer<PointLight>& obj)
{
	auto beg = scenePLights.begin();
	auto end = scenePLights.end();
	auto it = std::find_if(beg, end, [&](SharedPointer<PointLight>& o) {
		return o->Compare(*obj);
		});

	if (it != end)
		scenePLights.erase(it);
}

template<>
void Scene::RemoveSceneObject(const SharedPointer<StaticMesh>& obj)
{
	auto beg = sceneSMeshes.begin();
	auto end = sceneSMeshes.end();
	auto it = std::find_if(beg, end, [&](SharedPointer<StaticMesh>& o) {
		return o->Compare(*obj);
		});

	if (it != end)
		sceneSMeshes.erase(it);
}


void Scene::DrawScene(Shader* shader)
{
	/* Render directional light */
	if (HasDirLight())
		if (sceneDLight->visible)
			sceneDLight->RenderLight(shader);

	/* Render point lights */
	for (auto& scenePLight : scenePLights)
		if (scenePLight->visible)
			scenePLight->RenderLight(shader);

	/* Render static mesh objects (enable cull face to improve performance) */
	for (auto& sceneSMesh : sceneSMeshes)
		if (sceneSMesh->visible)
			sceneSMesh->Draw(shader);
}

/* -----------------------------------------------------
 *          PRIVATE METHODS
 * -----------------------------------------------------
*/




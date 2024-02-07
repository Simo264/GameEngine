#include "Scene.hh"
#include "Shader.hh"

void Scene::DrawScene(Shader* shader)
{
	/* Render directional light */
	if (HasDirLight() && sceneDLight->visible)
		sceneDLight->object->RenderLight(shader);

	/* Render point lights */
	for (auto ptr : scenePLights)
		if (ptr->visible)
			ptr->object->RenderLight(shader);
	
	/* Render static mesh objects */
	//glEnable(GL_CULL_FACE);
	for (auto ptr : sceneSMeshes)
		if (ptr->visible)
			ptr ->object->Draw(shader);
	//glDisable(GL_CULL_FACE);
}

void Scene::AddDirectionalLight(SceneObject<DirectionalLight>& dirLight)
{
	sceneDLight = &dirLight;
}

void Scene::RemoveDirectionalLight()
{
	sceneDLight = nullptr;
}

void Scene::AddPointLight(SceneObject<PointLight>& pointLight)
{
	scenePLights.push_back(&pointLight);
}

void Scene::RemovePointLight(SceneObject<PointLight>& pointLight)
{
	auto beg = scenePLights.begin();
	auto end = scenePLights.end();
	auto it = std::find_if(beg, end, [&](SceneObject<PointLight>* ptr) {
		return ptr->Compare(pointLight);
		});

	if (it != end)
		scenePLights.erase(it);
}

void Scene::AddStaticMesh(SceneObject<StaticMesh>& staticMesh)
{
	//_sMeshes.push_back(&staticMesh);
}

void Scene::RemoveStaticMesh(SceneObject<StaticMesh>& staticMesh)
{
	//auto beg = _pLights.begin();
	//auto end = _pLights.end();
	//auto it = std::find_if(beg, end, [&](SceneObject<StaticMesh>* ptr) {
	//	return ptr->Compare(staticMesh);
	//	});

	//if (it != end)
	//	_pLights.erase(it);
}


#include "Scene.hh"
#include "Graphics/Shader.hh"
#include "Mesh/StaticMesh.hh"

#include "Lighting/DirectionalLight.hh"
#include "Lighting/PointLight.hh"
#include "Lighting/SpotLight.hh"

void Scene::DrawScene(Shader* shader)
{
	if (directionalLight)
		directionalLight->RenderLight(shader);

	for (auto light : pointLights)
		light->RenderLight(shader);

	for (auto mesh : meshes)
		mesh->Draw(shader);
}

void Scene::AddStaticMesh(StaticMesh* staticMesh)
{
	meshes.push_back(staticMesh);
}

void Scene::AddPointLight(PointLight* pointLight)
{
	pointLights.push_back(pointLight);
}


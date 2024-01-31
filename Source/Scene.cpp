#include "Scene.hh"
#include "Shader.hh"
#include "Mesh/StaticMesh.hh"
#include "Mesh/InstancingMesh.hh"

#include "Lighting/DirectionalLight.hh"
#include "Lighting/PointLight.hh"
#include "Lighting/SpotLight.hh"

void Scene::DrawScene(Shader* shader)
{
	if (directionalLight)
		directionalLight->RenderLight(shader);

	for (auto light : pointLights)
		light->RenderLight(shader);

	/* Enable culling for meshes */
	//glEnable(GL_CULL_FACE);
	for (auto mesh : statMeshes)
		mesh->Draw(shader);
	//glDisable(GL_CULL_FACE);
}

void Scene::AddStaticMesh(StaticMesh* staticMesh)
{
	statMeshes.push_back(staticMesh);
}

void Scene::AddPointLight(PointLight* pointLight)
{
	pointLights.push_back(pointLight);
}


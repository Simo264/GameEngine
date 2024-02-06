#include "Scene.hh"
#include "Shader.hh"
#include "Mesh/StaticMesh.hh"
#include "Mesh/InstancingMesh.hh"

void Scene::DrawScene(Shader* shader)
{
	if (dLight.object && dLight.visible)
		dLight.object->RenderLight(shader);

	for (auto& sceneObj : pLights)
		if(sceneObj.visible)
			sceneObj.object->RenderLight(shader);

	/* Enable culling for meshes */
	//glEnable(GL_CULL_FACE);
	for (auto& sceneObj : sMeshes)
		if (sceneObj.visible)
			sceneObj.object->Draw(shader);
	//glDisable(GL_CULL_FACE);
}

void Scene::AddStaticMesh(StaticMesh* staticMesh)
{
	sMeshes.push_back({ staticMesh, true });
}

void Scene::AddPointLight(PointLight* pointLight)
{
	pLights.push_back({ pointLight, true} );
}

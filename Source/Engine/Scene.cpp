#include "Scene.hpp"
#include "Core/Log/Logger.hpp"
#include "Engine/ECS/GameObject.hpp"
#include "Engine/ECS/Components.hpp"
#include "Engine/ECS/Systems.hpp"

#include "Engine/SceneSerializer.hpp"

/* -----------------------------------------------------
 *          PUBLIC METHODS
 * -----------------------------------------------------
*/

GameObject Scene::CreateObject(const char* label, uint32_t type)
{
	GameObject object = GameObject(_registry.create(), &_registry);
	object.AddComponent<LabelComponent>(label);
	object.AddComponent<TypeComponent>(type);
	return object;
}

void Scene::DestroyObject(GameObject& object)
{
	/* Destroy mesh buffers and free memory */
	auto mesh = object.GetComponent<StaticMeshComponent>();
	if (mesh)
		mesh->DestroyMesh();

	_registry.destroy(object.GetObjectID());
}

void Scene::DrawScene(Shader& shader)
{
	/* Render lights */
	for (auto [entity, dLightComp] : _registry.view<DirLightComponent>().each())
		RenderLight(dLightComp, shader);
	for (auto [entity, pLightComp] : _registry.view<PointLightComponent>().each())
		RenderLight(pLightComp, shader);
	for (auto [entity, sLightComp] : _registry.view<SpotLightComponent>().each())
		RenderLight(sLightComp, shader);

	/* Render static meshes */
	for (auto [entity, smeshComp] : _registry.view<StaticMeshComponent>().each())
	{
		GameObject o{ entity, &_registry };

		auto transComp = o.GetComponent<TransformComponent>();
		Mat4f transform = Mat4f(1.0f);

		if (transComp)
			transform = transComp->GetTransformation();
		
		RenderMesh(smeshComp, transform, shader);
	}
}

void Scene::ClearScene()
{
	/* Destroy all game objects */
	for (auto [entity, lComp] : _registry.view<LabelComponent>().each())
	{
		GameObject o{ entity, &_registry };
		
		/* Destroy static mesh data (if exists) from GPU */
		if(auto smesh = o.GetComponent<StaticMeshComponent>())
			smesh->DestroyMesh();

		o.Destroy();
	}
	
	/* Clear scene */
	_registry.clear();
}

void Scene::LoadScene(const Path& filepath)
{
	SceneSerializer serializer;
	serializer.DeserializeScene(*this, filepath);
}

void Scene::SaveScene(const Path& filepath)
{
	SceneSerializer serializer;
	serializer.SerializeScene(*this, filepath);
}

/* -----------------------------------------------------
 *          PRIVATE METHODS
 * -----------------------------------------------------
*/


#include "Scene.hpp"
#include "Core/Log/Logger.hpp"
#include "Engine/GameObject.hpp"

#include "Engine/ECS/LabelComponent.hpp"
#include "Engine/ECS/TypeComponent.hpp"
#include "Engine/ECS/TransformComponent.hpp"
#include "Engine/ECS/StaticMeshComponent.hpp"
#include "Engine/ECS/Lighting/DirLightComponent.hpp"
#include "Engine/ECS/Lighting/PointLightComponent.hpp"
#include "Engine/ECS/Lighting/SpotLightComponent.hpp"

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

void Scene::DrawScene(Shader* shader)
{
	/* Render lights */
	for (auto [entity, dLightComp] : _registry.view<DirLightComponent>().each())
		dLightComp.Render(shader);
	for (auto [entity, pLightComp] : _registry.view<PointLightComponent>().each())
		pLightComp.Render(shader);
	for (auto [entity, sLightComp] : _registry.view<SpotLightComponent>().each())
		sLightComp.Render(shader);

	/* Render static meshes */
	for (auto [entity, smeshComp, transComp] : _registry.view<StaticMeshComponent, TransformComponent>().each())
		smeshComp.Draw(transComp.GetTransformation(), shader);
}

void Scene::ClearScene()
{
	/* Destroy buffers and free memory */
	for (auto [entity, smesh] : _registry.view<StaticMeshComponent>().each())
		smesh.DestroyMesh();

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


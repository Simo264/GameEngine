#include "Scene.hpp"

#include "Core/OpenGL.hpp"
#include "Core/Log/Logger.hpp"

#include "Engine/GameObject.hpp"
#include "Engine/Components.hpp"

#include "Engine/Graphics/Shader.hpp"
#include "Engine/SceneSerializer.hpp"

/* -----------------------------------------------------
 *          PUBLIC METHODS
 * -----------------------------------------------------
*/

Scene::Scene(const fs::path& filepath)
{
	LoadScene(filepath);
}

GameObject Scene::CreateObject()
{
	GameObject object = GameObject(_registry.create(), &_registry);
	object.AddComponent<Components::Label>("Object");
	return object;
}

void Scene::DestroyObject(GameObject& object)
{
	_registry.destroy(object.GetObjectID());
}

void Scene::ClearScene()
{
	for (auto [entity, lComp] : _registry.view<Components::Label>().each())
	{
		GameObject object{ entity, &_registry };
		
		/* Free GPU memory */
		if(auto* model = object.GetComponent<Components::Model>())
			model->DestroyModel();
		if (auto* mesh = object.GetComponent<Components::Mesh>())
			mesh->DestroyMesh();

		object.Invalidate();
	}
	
	/* Clear scene */
	_registry.clear();
}

void Scene::LoadScene(const fs::path& filepath)
{
	SceneSerializer serializer;
	serializer.DeserializeScene(*this, filepath);
}

void Scene::SaveScene(const fs::path& filepath)
{
	SceneSerializer serializer;
	serializer.SerializeScene(*this, filepath);
}

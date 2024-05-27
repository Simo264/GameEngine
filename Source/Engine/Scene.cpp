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

GameObject Scene::CreateObject()
{
	GameObject object = GameObject(_registry.create(), &_registry);
	object.AddComponent<LabelComponent>("Object");
	return object;
}

void Scene::DestroyObject(GameObject& object)
{
	_registry.destroy(object.GetObjectID());
}

void Scene::ClearScene()
{
	for (auto [entity, lComp] : _registry.view<LabelComponent>().each())
	{
		GameObject object{ entity, &_registry };
		
		/* Free GPU memory */
		if(auto smesh = object.GetComponent<StaticMeshComponent>())
			smesh->DestroyMesh();

		object.Invalidate();
	}
	
	/* Clear scene */
	_registry.clear();
}

void Scene::LoadScene(const fspath& filepath)
{
	SceneSerializer serializer;
	serializer.DeserializeScene(*this, filepath);
}

void Scene::SaveScene(const fspath& filepath)
{
	SceneSerializer serializer;
	serializer.SerializeScene(*this, filepath);
}

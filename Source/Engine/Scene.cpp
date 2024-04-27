#include "Scene.hpp"
#include "Core/Log/Logger.hpp"

#include "Engine/GameObject.hpp"
#include "Engine/Components.hpp"
#include "Engine/ShaderUniforms.hpp"

#include "Engine/Graphics/Shader.hpp"
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
	_registry.destroy(object.GetObjectID());
}

void Scene::DrawScene(Program& program)
{
	/* Render lights */
	for (auto [entity, dLightComp] : _registry.view<DirLightComponent>().each())
		dLightComp.RenderLight(program);

	//for (auto [entity, pLightComp] : _registry.view<PointLightComponent>().each())
	//	pLightComp.RenderLight(shader);
	
	//for (auto [entity, sLightComp] : _registry.view<SpotLightComponent>().each())
	//	sLightComp.RenderLight(shader);

	/* Render meshes */
	for (auto [entity, smeshComp] : _registry.view<StaticMeshComponent>().each())
	{
		GameObject object{ entity, &_registry };
		
		Mat4f transform = Mat4f(1.0f);
		if (TransformComponent* transComp = object.GetComponent<TransformComponent>())
			transform = transComp->GetTransformation();
		
		program.SetUniformMat4f(SHADER_UNIFORM_MODEL, transform);
		smeshComp.DrawMesh();
	}
}

void Scene::ClearScene()
{
	/* Destroy all game objects */
	for (auto [entity, lComp] : _registry.view<LabelComponent>().each())
	{
		GameObject o{ entity, &_registry };
		
		/* Free GPU memory */
		if(auto smesh = o.GetComponent<StaticMeshComponent>())
			smesh->DestroyMesh();

		o.Invalidate();
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


/* -----------------------------------------------------
 *          PRIVATE METHODS
 * -----------------------------------------------------
*/


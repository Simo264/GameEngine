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
	for (auto [entity, smeshComp] : _registry.view<StaticMeshComponent>().each())
	{
		GameObject obj{ entity, &_registry };
		TransformComponent* transform = obj.GetComponent<TransformComponent>();
		smeshComp.Draw(transform->GetTransformation(), shader);
	}
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

#if 0
void Scene::LoadScene(Path filepath)
{
	INIFileParser conf(filepath);
	conf.ReadData();

	for (auto const& it : conf.GetData())
	{
		auto const& section = it.first;
		auto const& collection = it.second;

		if (section.find("directional_light") != std::string::npos)
		{
			SharedPointer<DirectionalLight> dLight = std::make_shared<DirectionalLight>("UDirLight");
			dLight->tagName = collection.get("tag");
			dLight->color = INIFileParser::StringToVec3<Vec3i>(collection.get("color"));
			dLight->ambient = std::stof(collection.get("ambient"));
			dLight->diffuse = std::stof(collection.get("diffuse"));
			dLight->specular = std::stof(collection.get("specular"));
			dLight->direction = INIFileParser::StringToVec3<Vec3f>(collection.get("direction"));
			AddSceneObject(dLight);
		}
		else if (section.find("point_light") != std::string::npos)
		{
			char uname[32]{};
			sprintf_s(uname, "UPointLight[%d]", (int) scenePLights.size());

			SharedPointer<PointLight> pLight = std::make_shared<PointLight>(uname);
			pLight->tagName = collection.get("tag");
			pLight->color = INIFileParser::StringToVec3<Vec3i>(collection.get("color"));
			pLight->ambient = std::stof(collection.get("ambient"));
			pLight->diffuse = std::stof(collection.get("diffuse"));
			pLight->specular = std::stof(collection.get("specular"));
			pLight->position = INIFileParser::StringToVec3<Vec3f>(collection.get("position"));
			AddSceneObject(pLight);
		}
		else if (section.find("static_mesh") != std::string::npos)
		{
			Path modelPath = collection.get("model-path");
			SharedPointer<StaticMesh> sMesh = std::make_shared<StaticMesh>(modelPath);
			sMesh->tagName = collection.get("tag");
			sMesh->transform.position = INIFileParser::StringToVec3<Vec3f>(collection.get("position"));
			sMesh->transform.scale = INIFileParser::StringToVec3<Vec3f>(collection.get("scale"));
			sMesh->transform.degrees = INIFileParser::StringToVec3<Vec3f>(collection.get("degrees"));

			if (collection.has("texture-diffuse"))
			{
				Path p = collection.get("texture-diffuse");
				sMesh->material.diffuse = TextureManager::Instance().GetTextureByPath(p);
			}

			AddSceneObject(sMesh);
		}
	}
}
#endif

#if 0
void Scene::SaveScene(Path outfile)
{
	INIFileParser conf(outfile);
	
	char section[32]{};
	if (HasDirLight())
	{
		sprintf_s(section, "directional_light_%d", sceneDLight->GetID());
		conf.Update(section, "tag", sceneDLight->tagName.c_str());
		conf.Update(section, "color", INIFileParser::Vec3ToString<Vec3i>(sceneDLight->color).c_str());
		conf.Update(section, "ambient", std::to_string(sceneDLight->ambient).c_str());
		conf.Update(section, "diffuse", std::to_string(sceneDLight->diffuse).c_str());
		conf.Update(section, "specular", std::to_string(sceneDLight->specular).c_str());
		conf.Update(section, "direction", INIFileParser::Vec3ToString<Vec3f>(sceneDLight->direction).c_str());
	}

	for (const auto& pLight : scenePLights)
	{
		sprintf_s(section, "point_light_%d", pLight->GetID());
		conf.Update(section, "tag", pLight->tagName.c_str());
		conf.Update(section, "color", INIFileParser::Vec3ToString<Vec3i>(pLight->color).c_str());
		conf.Update(section, "ambient", std::to_string(pLight->ambient).c_str());
		conf.Update(section, "diffuse", std::to_string(pLight->diffuse).c_str());
		conf.Update(section, "specular", std::to_string(pLight->specular).c_str());
		conf.Update(section, "position", INIFileParser::Vec3ToString<Vec3f>(pLight->position).c_str());
	}
	
	for (const auto& sMesh : sceneSMeshes)
	{
		sprintf_s(section, "static_mesh_%d", sMesh->GetID());
		conf.Update(section, "tag", sMesh->tagName.c_str());
		conf.Update(section, "model-path", sMesh->modelPath.string().c_str());
		conf.Update(section, "position", INIFileParser::Vec3ToString<Vec3f>(sMesh->transform.position).c_str());
		conf.Update(section, "scale", INIFileParser::Vec3ToString<Vec3f>(sMesh->transform.scale).c_str());
		conf.Update(section, "degrees", INIFileParser::Vec3ToString<Vec3f>(sMesh->transform.degrees).c_str());

		if(sMesh->material.diffuse)
			conf.Update(section, "texture-diffuse", sMesh->material.diffuse->texturePath.string().c_str());
	}

	conf.Write();
}
#endif


/* -----------------------------------------------------
 *          PRIVATE METHODS
 * -----------------------------------------------------
*/


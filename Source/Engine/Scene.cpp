#include "Scene.hpp"
#include "Engine/Graphics/Shader.hpp"
#include "Engine/Graphics/Texture2D.hpp"
#include "Engine/Subsystems/TexturesManager.hpp"

#include "Core/FileParser/INIFileParser.hpp"
#include "Core/Log/Logger.hpp"

/* -----------------------------------------------------
 *          PUBLIC METHODS
 * -----------------------------------------------------
*/

void Scene::ClearScene() 
{ 
	sceneDLight.reset();

	for (auto& pLight : scenePLights)
		pLight.reset();
	scenePLights.clear();

	for (auto& sMesh: sceneSMeshes)
		sMesh.reset();
	sceneSMeshes.clear();
}

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
				sMesh->material.diffuse = TexturesManager::Instance().GetTextureByPath(p);
			}

			AddSceneObject(sMesh);
		}
	}
}

void Scene::SaveScene(Path outfile)
{
	if (IsEmpty())
	{
		CONSOLE_WARN("Scene is empty!");
		return;
	}

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

void Scene::DrawScene(Shader* shader)
{
	/* Render directional light */
	if (HasDirLight())
		if (sceneDLight->visible)
			sceneDLight->RenderLight(shader);

	/* Render point lights */
	for (auto& scenePLight : scenePLights)
		if (scenePLight->visible)
			scenePLight->RenderLight(shader);

	/* Render static mesh objects (enable cull face to improve performance) */
	for (auto& sceneSMesh : sceneSMeshes)
		if (sceneSMesh->visible)
			sceneSMesh->Draw(shader);
}

void Scene::AddSceneObject(const SharedPointer<DirectionalLight>& obj)
{
	sceneDLight = obj;
}

void Scene::AddSceneObject(const SharedPointer<PointLight>& obj)
{
	scenePLights.push_back(obj);
}

void Scene::AddSceneObject(const SharedPointer<StaticMesh>& obj)
{
	sceneSMeshes.push_back(obj);
}

void Scene::RemoveSceneObject(const SharedPointer<DirectionalLight>& obj)
{
	sceneDLight = nullptr;
}

void Scene::RemoveSceneObject(const SharedPointer<PointLight>& obj)
{
	auto beg = scenePLights.begin();
	auto end = scenePLights.end();
	auto it = std::find_if(beg, end, [&](SharedPointer<PointLight>& o) {
		return o->Compare(*obj);
		});

	if (it != end)
		scenePLights.erase(it);
}

void Scene::RemoveSceneObject(const SharedPointer<StaticMesh>& obj)
{
	auto beg = sceneSMeshes.begin();
	auto end = sceneSMeshes.end();
	auto it = std::find_if(beg, end, [&](SharedPointer<StaticMesh>& o) {
		return o->Compare(*obj);
		});

	if (it != end)
		sceneSMeshes.erase(it);
}

/* -----------------------------------------------------
 *          PRIVATE METHODS
 * -----------------------------------------------------
*/


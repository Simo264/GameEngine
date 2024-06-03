#include "SceneSerializer.hpp"

#include "Engine/Scene.hpp"
#include "Engine/ObjectLoader.hpp"

#include "Engine/GameObject.hpp"
#include "Engine/Components.hpp"


#include "Engine/Graphics/Texture2D.hpp"
#include "Engine/Subsystems/TextureManager.hpp"

#include "Core/FileParser/INIFileParser.hpp"


/* -----------------------------------------------------
 *          PUBLIC METHODS
 * -----------------------------------------------------
*/

void SceneSerializer::SerializeScene(Scene& scene, const fspath& filepath)
{
	INIFileParser conf(filepath);

	for (auto [entity, lComp] : scene.Reg().view<LabelComponent>().each())
	{
		GameObject object{ entity, &scene.Reg() };
		const uint32_t objectID = static_cast<uint32_t>(object.GetObjectID());

		string section = std::format("entity{}:LabelComponent", objectID);
		conf.Update(section.c_str(), "label", lComp.label.c_str());

		if (auto light = object.GetComponent<DirLightComponent>())
		{
			string section = std::format("entity{}:DirLightComponent", objectID);
			conf.Update(section.c_str(), "color", std::format("{},{},{}", light->color.x, light->color.y, light->color.z).c_str());
			conf.Update(section.c_str(), "ambient", std::format("{}", light->ambient).c_str());
			conf.Update(section.c_str(), "diffuse", std::format("{}", light->diffuse).c_str());
			conf.Update(section.c_str(), "specular", std::format("{}", light->specular).c_str());
			conf.Update(section.c_str(), "direction", std::format("{},{},{}", light->direction.x, light->direction.y, light->direction.z).c_str());
		}
		if (auto light = object.GetComponent<PointLightComponent>())
		{
			string section = std::format("entity{}:PointLightComponent", objectID);
			conf.Update(section.c_str(), "color", std::format("{},{},{}", light->color.x, light->color.y, light->color.z).c_str());
			conf.Update(section.c_str(), "ambient", std::to_string(light->ambient).c_str());
			conf.Update(section.c_str(), "diffuse", std::to_string(light->diffuse).c_str());
			conf.Update(section.c_str(), "specular", std::to_string(light->specular).c_str());
			conf.Update(section.c_str(), "position", std::format("{},{},{}", light->position.x, light->position.y, light->position.z).c_str());
			conf.Update(section.c_str(), "linear", std::to_string(light->linear).c_str());
			conf.Update(section.c_str(), "quadratic", std::to_string(light->quadratic).c_str());
		}
		if (auto light = object.GetComponent<SpotLightComponent>())
		{
			string section = std::format("entity{}:SpotLightComponent", objectID);
			conf.Update(section.c_str(), "color", std::format("{},{},{}", light->color.x, light->color.y, light->color.z).c_str());
			conf.Update(section.c_str(), "ambient", std::to_string(light->ambient).c_str());
			conf.Update(section.c_str(), "diffuse", std::to_string(light->diffuse).c_str());
			conf.Update(section.c_str(), "specular", std::to_string(light->specular).c_str());
			conf.Update(section.c_str(), "direction", std::format("{},{},{}", light->direction.x, light->direction.y, light->direction.z).c_str());
			conf.Update(section.c_str(), "position", std::format("{},{},{}", light->position.x, light->position.y, light->position.z).c_str());
			conf.Update(section.c_str(), "linear", std::to_string(light->linear).c_str());
			conf.Update(section.c_str(), "quadratic", std::to_string(light->quadratic).c_str());
			conf.Update(section.c_str(), "cutoff", std::to_string(light->cutOff).c_str());
			conf.Update(section.c_str(), "outercutoff", std::to_string(light->outerCutOff).c_str());
		}
		if (auto transform = object.GetComponent<TransformComponent>())
		{
			string section = std::format("entity{}:TransformComponent", objectID);
			conf.Update(section.c_str(), "position", std::format("{},{},{}", transform->position.x, transform->position.y, transform->position.z).c_str());
			conf.Update(section.c_str(), "scale", std::format("{},{},{}", transform->scale.x, transform->scale.y, transform->scale.z).c_str());
			conf.Update(section.c_str(), "rotation", std::format("{},{},{}", transform->rotation.x, transform->rotation.y, transform->rotation.z).c_str());
		}
		if (auto mesh = object.GetComponent<StaticMeshComponent>())
		{
			string section = std::format("entity{}:StaticMeshComponent", objectID);
			string diffuse = (mesh->material.diffuse ? mesh->material.diffuse->path.string() : "");
			string specular = (mesh->material.specular ? mesh->material.specular->path.string() : "");
			conf.Update(section.c_str(), "model_path", mesh->modelPath.string().c_str());
			conf.Update(section.c_str(), "material_diffuse", diffuse.c_str());
			conf.Update(section.c_str(), "material_specular", specular.c_str());
		}
	
	}
	conf.Generate(true);
}
void SceneSerializer::DeserializeScene(Scene& scene, const fspath& filepath)
{ 
	INIFileParser conf(filepath);
	conf.ReadData();
	
	GameObject object{ entt::null, nullptr };
	uint32_t currentObjectId = static_cast<uint32_t>(-1);

	auto instanceTM = TextureManager::Instance();

	for (auto const& it : conf.GetData())
	{
		const string& section = it.first;
		const char* dots = strchr(section.c_str(), ':');
		const int dotsPosition = static_cast<int>(dots - section.c_str());
		const char* componentName = dots + 1;
		constexpr int entityWordLen = 6;
		uint32_t objectId = std::stoul(section.substr(entityWordLen, dotsPosition - entityWordLen));
		
		if (currentObjectId != objectId)
		{
			currentObjectId = objectId;
			object = scene.CreateObject();
		}

		if (std::strcmp(componentName, "labelcomponent") == 0)
		{
			string label = conf.GetValue(section.c_str(), "label");
			object.GetComponent<LabelComponent>()->label = label;
		}
		else if (std::strcmp(componentName, "transformcomponent") == 0)
		{
			string strPosition = conf.GetValue(section.c_str(), "position");
			string strRotation = conf.GetValue(section.c_str(), "rotation");
			string strScale = conf.GetValue(section.c_str(), "scale");
			
			auto& transform = object.AddComponent<TransformComponent>();
			transform.position = INIFileParser::StringToVec3f(strPosition);
			transform.rotation = INIFileParser::StringToVec3f(strRotation);
			transform.scale = INIFileParser::StringToVec3f(strScale);
			transform.UpdateTransformation();
		}
		else if (std::strcmp(componentName, "staticmeshcomponent") == 0)
		{
			fspath model = fspath(conf.GetValue(section.c_str(), "model_path")).lexically_normal();
			fspath diffuse = fspath(conf.GetValue(section.c_str(), "material_diffuse	")).lexically_normal();
			fspath specular = fspath(conf.GetValue(section.c_str(), "material_specular")).lexically_normal();

			auto& mesh = object.AddComponent<StaticMeshComponent>(model);
			if (!diffuse.empty())
				mesh.material.diffuse = instanceTM->GetTextureByPath(diffuse);
			if(!specular.empty())
				mesh.material.specular = instanceTM->GetTextureByPath(specular);
		}
		else if (std::strcmp(componentName, "dirlightcomponent") == 0)
		{
			string strColor = conf.GetValue(section.c_str(), "color");
			string strDirection = conf.GetValue(section.c_str(), "direction");

			auto& light = object.AddComponent<DirLightComponent>();
			light.color = INIFileParser::StringToVec3f(strColor);
			light.ambient = std::stof(conf.GetValue(section.c_str(), "ambient"));
			light.diffuse = std::stof(conf.GetValue(section.c_str(), "diffuse"));
			light.specular = std::stof(conf.GetValue(section.c_str(), "specular"));
			light.direction = INIFileParser::StringToVec3f(strDirection);
		}
		else if (std::strcmp(componentName, "pointlightcomponent") == 0)
		{
			string strColor = conf.GetValue(section.c_str(), "color");
			string strPosition = conf.GetValue(section.c_str(), "position");

			auto& light = object.AddComponent<PointLightComponent>();
			light.color = INIFileParser::StringToVec3f(strColor);
			light.ambient = std::stof(conf.GetValue(section.c_str(), "ambient"));
			light.diffuse = std::stof(conf.GetValue(section.c_str(), "diffuse"));
			light.specular = std::stof(conf.GetValue(section.c_str(), "specular"));
			light.position = INIFileParser::StringToVec3f(strPosition);
		}
		else if (std::strcmp(componentName, "spotlightcomponent") == 0)
		{
			string strColor = conf.GetValue(section.c_str(), "color");
			string strPosition = conf.GetValue(section.c_str(), "position");
			string strDirection = conf.GetValue(section.c_str(), "direction");

			auto& light = object.AddComponent<SpotLightComponent>();
			light.color = INIFileParser::StringToVec3f(strColor);
			light.ambient = std::stof(conf.GetValue(section.c_str(), "ambient"));
			light.diffuse = std::stof(conf.GetValue(section.c_str(), "diffuse"));
			light.specular = std::stof(conf.GetValue(section.c_str(), "specular"));
			light.position = INIFileParser::StringToVec3f(strPosition);
			light.direction = INIFileParser::StringToVec3f(strDirection);
			light.linear = std::stof(conf.GetValue(section.c_str(), "linear"));
			light.quadratic = std::stof(conf.GetValue(section.c_str(), "quadratic"));
			light.cutOff = std::stof(conf.GetValue(section.c_str(), "cutoff"));
			light.outerCutOff = std::stof(conf.GetValue(section.c_str(), "outercutoff"));
		}
	}
}

/* -----------------------------------------------------
 *          PRIVATE METHODS
 * -----------------------------------------------------
*/

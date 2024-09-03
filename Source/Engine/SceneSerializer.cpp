#include "SceneSerializer.hpp"

#include "Engine/Filesystem/ConfigFile.hpp"

#include "Engine/Utils.hpp"
#include "Engine/Scene.hpp"
#include "Engine/GameObject.hpp"
#include "Engine/Components.hpp"

#include "Engine/Graphics/Objects/Texture2D.hpp"
#include "Engine/Subsystems/TextureManager.hpp"

/* -----------------------------------------------------
 *          PUBLIC METHODS
 * -----------------------------------------------------
*/

void SceneSerializer::SerializeScene(Scene& scene, const fs::path& filepath)
{
	ConfigFile conf(filepath);

	for (auto [entity, label] : scene.Reg().view<Components::Label>().each())
	{
		GameObject object{ entity, &scene.Reg() };
		const u32 objectID = static_cast<u32>(object.GetObjectID());

		String section = std::format("entity{}:Label", objectID);
		conf.Update(section.c_str(), "label", label.value.c_str());
		
		if (auto* light = object.GetComponent<Components::DirectionalLight>())
		{
			String section = std::format("entity{}:DirectionalLight", objectID);
			String color = std::format("{},{},{}", light->color.x, light->color.y, light->color.z);
			String diffuseIntensity = std::to_string(light->diffuseIntensity);
			String specularIntensity = std::to_string(light->specularIntensity);
			String direction = std::format("{},{},{}", light->direction.x, light->direction.y, light->direction.z);

			conf.Update(section.c_str(), "color", color.c_str());
			conf.Update(section.c_str(), "diffuseIntensity", diffuseIntensity.c_str());
			conf.Update(section.c_str(), "specularIntensity", specularIntensity.c_str());
			conf.Update(section.c_str(), "direction", direction.c_str());
		}
		if (auto* light = object.GetComponent<Components::PointLight>())
		{
			String section = std::format("entity{}:PointLight", objectID);
			String color = std::format("{},{},{}", light->color.x, light->color.y, light->color.z);
			String diffuseIntensity = std::to_string(light->diffuseIntensity);
			String specularIntensity = std::to_string(light->specularIntensity);
			String position = std::format("{},{},{}", light->position.x, light->position.y, light->position.z);
			String kl = std::to_string(light->attenuation.kl);
			String kq = std::to_string(light->attenuation.kq);

			conf.Update(section.c_str(), "color", color.c_str());
			conf.Update(section.c_str(), "diffuseIntensity", diffuseIntensity.c_str());
			conf.Update(section.c_str(), "specularIntensity", specularIntensity.c_str());
			conf.Update(section.c_str(), "position", position.c_str());
			conf.Update(section.c_str(), "attenuation.kl", kl.c_str());
			conf.Update(section.c_str(), "attenuation.kq", kq.c_str());
		}
		if (auto* light = object.GetComponent<Components::SpotLight>())
		{
			String section = std::format("entity{}:SpotLight", objectID);
			String color = std::format("{},{},{}", light->color.x, light->color.y, light->color.z);
			String diffuseIntensity = std::to_string(light->diffuseIntensity);
			String specularIntensity = std::to_string(light->specularIntensity);
			String direction = std::format("{},{},{}", light->direction.x, light->direction.y, light->direction.z);
			String position = std::format("{},{},{}", light->position.x, light->position.y, light->position.z);
			String cutOff = std::to_string(light->cutOff);
			String outerCutOff = std::to_string(light->outerCutOff);
			String kl = std::to_string(light->attenuation.kl);
			String kq = std::to_string(light->attenuation.kq);

			conf.Update(section.c_str(), "color", color.c_str());
			conf.Update(section.c_str(), "diffuseIntensity", diffuseIntensity.c_str());
			conf.Update(section.c_str(), "specularIntensity", specularIntensity.c_str());
			conf.Update(section.c_str(), "direction", direction.c_str());
			conf.Update(section.c_str(), "position", position.c_str());
			conf.Update(section.c_str(), "attenuation.kl", kl.c_str());
			conf.Update(section.c_str(), "attenuation.kq", kq.c_str());
			conf.Update(section.c_str(), "cutOff", cutOff.c_str());
			conf.Update(section.c_str(), "outerCutOff", outerCutOff.c_str());
		}
		if (auto* transform = object.GetComponent<Components::Transform>())
		{
			String section = std::format("entity{}:Transform", objectID);
			conf.Update(section.c_str(), "position", std::format("{},{},{}", transform->position.x, transform->position.y, transform->position.z).c_str());
			conf.Update(section.c_str(), "scale", std::format("{},{},{}", transform->scale.x, transform->scale.y, transform->scale.z).c_str());
			conf.Update(section.c_str(), "rotation", std::format("{},{},{}", transform->rotation.x, transform->rotation.y, transform->rotation.z).c_str());
		}
		if (auto* model = object.GetComponent<Components::Model>())
		{
			String section = std::format("entity{}:Model", objectID);
			conf.Update(section.c_str(), "path", model->modelPath.string().c_str());
		}
	}
	conf.Generate(true);
}
void SceneSerializer::DeserializeScene(Scene& scene, const fs::path& filepath)
{ 
	CONSOLE_INFO("Loading scene {}...", filepath.string());

	ConfigFile conf(filepath);
	conf.ReadData();
	
	GameObject object{ entt::null, nullptr };
	u32 currentObjectId = static_cast<u32>(-1);

	for (const auto& it : conf.GetData())
	{
		const String& section = it.first;
		const char* dots = strchr(section.c_str(), ':');
		const i32 dotsPosition = static_cast<i32>(dots - section.c_str());
		const char* componentName = dots + 1;
		constexpr i32 entityWordLen = 6;
		u32 objectId = std::stoul(section.substr(entityWordLen, dotsPosition - entityWordLen));
		
		if (currentObjectId != objectId)
		{
			currentObjectId = objectId;
			object = scene.CreateObject();
		}

		if (std::strcmp(componentName, "Label") == 0)
		{
			String label = conf.GetValue(section.c_str(), "label");
			object.GetComponent<Components::Label>()->value = label;
		}
		else if (std::strcmp(componentName, "Transform") == 0)
		{
			String position = conf.GetValue(section.c_str(), "position");
			String rotation = conf.GetValue(section.c_str(), "rotation");
			String scale = conf.GetValue(section.c_str(), "scale");
			
			auto& transform = object.AddComponent<Components::Transform>();
			transform.position = Utils::StringToVec3f(position);
			transform.rotation = Utils::StringToVec3f(rotation);
			transform.scale = Utils::StringToVec3f(scale);
			transform.UpdateTransformation();
		}
		else if (std::strcmp(componentName, "Model") == 0)
		{
			String path = conf.GetValue(section.c_str(), "path");
			fs::path modelPath = fs::path(path).lexically_normal();
			auto& model = object.AddComponent<Components::Model>(modelPath);
		}
		else if (std::strcmp(componentName, "DirectionalLight") == 0)
		{
			String color = conf.GetValue(section.c_str(), "color");
			String diffuseIntensity = conf.GetValue(section.c_str(), "diffuseIntensity");
			String specularIntensity = conf.GetValue(section.c_str(), "specularIntensity");
			String direction = conf.GetValue(section.c_str(), "direction");

			auto& light = object.AddComponent<Components::DirectionalLight>();
			light.color = Utils::StringToVec3f(color);
			light.diffuseIntensity = std::stof(diffuseIntensity);
			light.specularIntensity = std::stof(specularIntensity);
			light.direction = Utils::StringToVec3f(direction);
		}
		else if (std::strcmp(componentName, "PointLight") == 0)
		{
			String color = conf.GetValue(section.c_str(), "color");
			String diffuseIntensity = conf.GetValue(section.c_str(), "diffuseIntensity");
			String specularIntensity = conf.GetValue(section.c_str(), "specularIntensity");
			String position = conf.GetValue(section.c_str(), "position");
			String kl = conf.GetValue(section.c_str(), "attenuation.kl");
			String kq = conf.GetValue(section.c_str(), "attenuation.kq");

			auto& light = object.AddComponent<Components::PointLight>();
			light.color = Utils::StringToVec3f(color);
			light.diffuseIntensity = std::stof(diffuseIntensity);
			light.specularIntensity = std::stof(specularIntensity);
			light.position = Utils::StringToVec3f(position);
			light.attenuation.kl = std::stof(kl);
			light.attenuation.kq = std::stof(kq);
		}
		else if (std::strcmp(componentName, "SpotLight") == 0)
		{
			String color = conf.GetValue(section.c_str(), "color");
			String diffuseIntensity = conf.GetValue(section.c_str(), "diffuseIntensity");
			String specularIntensity = conf.GetValue(section.c_str(), "specularIntensity");
			String position = conf.GetValue(section.c_str(), "position");
			String direction = conf.GetValue(section.c_str(), "direction");
			String kl = conf.GetValue(section.c_str(), "attenuation.kl");
			String kq = conf.GetValue(section.c_str(), "attenuation.kq");
			String cutOff = conf.GetValue(section.c_str(), "cutOff");
			String outerCutOff = conf.GetValue(section.c_str(), "outerCutOff");

			auto& light = object.AddComponent<Components::SpotLight>();
			light.color = Utils::StringToVec3f(color);
			light.diffuseIntensity = std::stof(diffuseIntensity);
			light.specularIntensity = std::stof(specularIntensity);
			light.position = Utils::StringToVec3f(position);
			light.direction = Utils::StringToVec3f(direction);
			light.attenuation.kl = std::stof(kl);
			light.attenuation.kq = std::stof(kq);
			light.cutOff = std::stof(cutOff);
			light.outerCutOff = std::stof(outerCutOff);
		}
	}
}

/* -----------------------------------------------------
 *          PRIVATE METHODS
 * -----------------------------------------------------
*/

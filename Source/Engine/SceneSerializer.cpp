#include "SceneSerializer.hpp"

#include "Engine/Filesystem/ConfigFile.hpp"

#include "Engine/Utils.hpp"
#include "Engine/Scene.hpp"
#include "Engine/GameObject.hpp"
#include "Engine/Components.hpp"

#include "Engine/Graphics/Objects/Texture2D.hpp"
#include "Engine/Subsystems/TextureManager.hpp"

void SerializeScene(Scene& scene, const fs::path& filepath)
{
	ConfigFile conf(filepath);

	for (auto [entity, label] : scene.Reg().view<Components::Label>().each())
	{
		GameObject object{ entity, &scene.Reg() };
		const u32 objectID = static_cast<u32>(object.GetObjectID());

		String section = std::format("entity{}:Label", objectID);
		conf.Update(section, "label", label.value);
		
		if (auto* light = object.GetComponent<Components::DirectionalLight>())
		{
			String section = std::format("entity{}:DirectionalLight", objectID);
			String color = std::format("{},{},{}", light->color.x, light->color.y, light->color.z);
			String diffuseIntensity = std::to_string(light->diffuseIntensity);
			String specularIntensity = std::to_string(light->specularIntensity);
			String direction = std::format("{},{},{}", light->direction.x, light->direction.y, light->direction.z);

			conf.Update(section, "color", color);
			conf.Update(section, "diffuseIntensity", diffuseIntensity);
			conf.Update(section, "specularIntensity", specularIntensity);
			conf.Update(section, "direction", direction);
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

			conf.Update(section, "color", color);
			conf.Update(section, "diffuseIntensity", diffuseIntensity);
			conf.Update(section, "specularIntensity", specularIntensity);
			conf.Update(section, "position", position);
			conf.Update(section, "attenuation.kl", kl);
			conf.Update(section, "attenuation.kq", kq);
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

			conf.Update(section, "color", color);
			conf.Update(section, "diffuseIntensity", diffuseIntensity);
			conf.Update(section, "specularIntensity", specularIntensity);
			conf.Update(section, "direction", direction);
			conf.Update(section, "position", position);
			conf.Update(section, "attenuation.kl", kl);
			conf.Update(section, "attenuation.kq", kq);
			conf.Update(section, "cutOff", cutOff);
			conf.Update(section, "outerCutOff", outerCutOff);
		}
		if (auto* transform = object.GetComponent<Components::Transform>())
		{
			String section = std::format("entity{}:Transform", objectID);
			conf.Update(section, "position", std::format("{},{},{}", transform->position.x, transform->position.y, transform->position.z));
			conf.Update(section, "scale", std::format("{},{},{}", transform->scale.x, transform->scale.y, transform->scale.z));
			conf.Update(section, "rotation", std::format("{},{},{}", transform->rotation.x, transform->rotation.y, transform->rotation.z));
		}
		if (auto* model = object.GetComponent<Components::Model>())
		{
			String section = std::format("entity{}:Model", objectID);
			conf.Update(section, "path", model->path.string());
		}
	}
	conf.Generate(true);
}

void DeserializeScene(Scene& scene, const fs::path& filepath)
{ 
	CONSOLE_INFO("Loading scene {}...", filepath.string());

	ConfigFile conf(filepath);
	conf.ReadData();
	
	GameObject object{ entt::null, nullptr };
	u32 currentObjectId = static_cast<u32>(entt::null);

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
			const String& label = conf.GetValue(section, "label");
			object.GetComponent<Components::Label>()->value = label;
		}
		else if (std::strcmp(componentName, "Transform") == 0)
		{
			const String& position = conf.GetValue(section, "position");
			const String& rotation = conf.GetValue(section, "rotation");
			const String& scale = conf.GetValue(section, "scale");
			
			auto& transform = object.AddComponent<Components::Transform>();
			transform.position = Utils::StringToVec3f(position);
			transform.rotation = Utils::StringToVec3f(rotation);
			transform.scale = Utils::StringToVec3f(scale);
			transform.UpdateTransformation();
		}
		else if (std::strcmp(componentName, "Model") == 0)
		{
			const String& path = conf.GetValue(section, "path");
			fs::path modelPath = fs::path(path).lexically_normal();
			auto& model = object.AddComponent<Components::Model>(modelPath);
		}
		else if (std::strcmp(componentName, "DirectionalLight") == 0)
		{
			const String& color = conf.GetValue(section, "color");
			const String& diffuseIntensity = conf.GetValue(section, "diffuseIntensity");
			const String& specularIntensity = conf.GetValue(section, "specularIntensity");
			const String& direction = conf.GetValue(section, "direction");

			auto& light = object.AddComponent<Components::DirectionalLight>();
			light.color = Utils::StringToVec3f(color);
			light.diffuseIntensity = std::stof(diffuseIntensity);
			light.specularIntensity = std::stof(specularIntensity);
			light.direction = Utils::StringToVec3f(direction);
		}
		else if (std::strcmp(componentName, "PointLight") == 0)
		{
			const String& color = conf.GetValue(section, "color");
			const String& diffuseIntensity = conf.GetValue(section, "diffuseIntensity");
			const String& specularIntensity = conf.GetValue(section, "specularIntensity");
			const String& position = conf.GetValue(section, "position");
			const String& kl = conf.GetValue(section, "attenuation.kl");
			const String& kq = conf.GetValue(section, "attenuation.kq");

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
			const String& color = conf.GetValue(section, "color");
			const String& diffuseIntensity = conf.GetValue(section, "diffuseIntensity");
			const String& specularIntensity = conf.GetValue(section, "specularIntensity");
			const String& position = conf.GetValue(section, "position");
			const String& direction = conf.GetValue(section, "direction");
			const String& kl = conf.GetValue(section, "attenuation.kl");
			const String& kq = conf.GetValue(section, "attenuation.kq");
			const String& cutOff = conf.GetValue(section, "cutOff");
			const String& outerCutOff = conf.GetValue(section, "outerCutOff");

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


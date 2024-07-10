#include "SceneSerializer.hpp"

#include "Core/FileParser/INIFileParser.hpp"

#include "Engine/Utils.hpp"
#include "Engine/Scene.hpp"
#include "Engine/ObjectLoader.hpp"
#include "Engine/GameObject.hpp"
#include "Engine/Components.hpp"

#include "Engine/Graphics/Texture2D.hpp"
#include "Engine/Subsystems/TextureManager.hpp"

/* -----------------------------------------------------
 *          PUBLIC METHODS
 * -----------------------------------------------------
*/

void SceneSerializer::SerializeScene(Scene& scene, const fspath& filepath)
{
	INIFileParser conf(filepath);

	for (auto [entity, label] : scene.Reg().view<Components::Label>().each())
	{
		GameObject object{ entity, &scene.Reg() };
		const uint32_t objectID = static_cast<uint32_t>(object.GetObjectID());

		string section = std::format("entity{}:Label", objectID);
		conf.Update(section.c_str(), "label", label.label.c_str());
		
		if (auto* light = object.GetComponent<Components::DirectionalLight>())
		{
			string section = std::format("entity{}:DirectionalLight", objectID);
			conf.Update(section.c_str(), "color", std::format("{},{},{}", light->color.x, light->color.y, light->color.z).c_str());
			conf.Update(section.c_str(), "diffuseIntensity", std::format("{}", light->diffuseIntensity).c_str());
			conf.Update(section.c_str(), "specularIntensity", std::format("{}", light->specularIntensity).c_str());
			conf.Update(section.c_str(), "direction", std::format("{},{},{}", light->direction.x, light->direction.y, light->direction.z).c_str());
		}
		if (auto* light = object.GetComponent<Components::PointLight>())
		{
			string section = std::format("entity{}:PointLight", objectID);
			conf.Update(section.c_str(), "color", std::format("{},{},{}", light->color.x, light->color.y, light->color.z).c_str());
			conf.Update(section.c_str(), "diffuseIntensity", std::to_string(light->diffuseIntensity).c_str());
			conf.Update(section.c_str(), "specularIntensity", std::to_string(light->specularIntensity).c_str());
			conf.Update(section.c_str(), "position", std::format("{},{},{}", light->position.x, light->position.y, light->position.z).c_str());
			conf.Update(section.c_str(), "attenuation.kl", std::to_string(light->attenuation.kl).c_str());
			conf.Update(section.c_str(), "attenuation.kq", std::to_string(light->attenuation.kq).c_str());
		}
		if (auto* light = object.GetComponent<Components::SpotLight>())
		{
			string section = std::format("entity{}:SpotLight", objectID);
			conf.Update(section.c_str(), "color", std::format("{},{},{}", light->color.x, light->color.y, light->color.z).c_str());
			conf.Update(section.c_str(), "diffuseIntensity", std::to_string(light->diffuseIntensity).c_str());
			conf.Update(section.c_str(), "specularIntensity", std::to_string(light->specularIntensity).c_str());
			conf.Update(section.c_str(), "direction", std::format("{},{},{}", light->direction.x, light->direction.y, light->direction.z).c_str());
			conf.Update(section.c_str(), "position", std::format("{},{},{}", light->position.x, light->position.y, light->position.z).c_str());
			conf.Update(section.c_str(), "attenuation.kl", std::to_string(light->attenuation.kl).c_str());
			conf.Update(section.c_str(), "attenuation.kq", std::to_string(light->attenuation.kq).c_str());
			conf.Update(section.c_str(), "cutOff", std::to_string(light->cutOff).c_str());
			conf.Update(section.c_str(), "outerCutOff", std::to_string(light->outerCutOff).c_str());
		}
		if (auto* transform = object.GetComponent<Components::Transform>())
		{
			string section = std::format("entity{}:Transform", objectID);
			conf.Update(section.c_str(), "position", std::format("{},{},{}", transform->position.x, transform->position.y, transform->position.z).c_str());
			conf.Update(section.c_str(), "scale", std::format("{},{},{}", transform->scale.x, transform->scale.y, transform->scale.z).c_str());
			conf.Update(section.c_str(), "rotation", std::format("{},{},{}", transform->rotation.x, transform->rotation.y, transform->rotation.z).c_str());
		}
		if (auto* model = object.GetComponent<Components::Model>())
		{
			string section = std::format("entity{}:Model", objectID);
			conf.Update(section.c_str(), "path", model->modelPath.string().c_str());
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

	auto* instanceTM = TextureManager::Instance();

	for (const auto& it : conf.GetData())
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

		if (std::strcmp(componentName, "Label") == 0)
		{
			string label = conf.GetValue(section.c_str(), "label");
			object.GetComponent<Components::Label>()->label = label;
		}
		else if (std::strcmp(componentName, "Transform") == 0)
		{
			string strPosition = conf.GetValue(section.c_str(), "position");
			string strRotation = conf.GetValue(section.c_str(), "rotation");
			string strScale = conf.GetValue(section.c_str(), "scale");
			
			auto& transform = object.AddComponent<Components::Transform>();
			transform.position = Utils::StringToVec3f(strPosition);
			transform.rotation = Utils::StringToVec3f(strRotation);
			transform.scale = Utils::StringToVec3f(strScale);
			transform.UpdateTransformation();
		}
		else if (std::strcmp(componentName, "Model") == 0)
		{
			fspath modelPath = fspath(conf.GetValue(section.c_str(), "path")).lexically_normal();
			auto& model = object.AddComponent<Components::Model>(modelPath);
		}
		else if (std::strcmp(componentName, "DirectionalLight") == 0)
		{
			string strColor = conf.GetValue(section.c_str(), "color");
			string strDirection = conf.GetValue(section.c_str(), "direction");

			auto& light = object.AddComponent<Components::DirectionalLight>();
			light.color = Utils::StringToVec3f(strColor);
			light.diffuseIntensity = std::stof(conf.GetValue(section.c_str(), "diffuseIntensity"));
			light.specularIntensity = std::stof(conf.GetValue(section.c_str(), "specularIntensity"));
			light.direction = Utils::StringToVec3f(strDirection);
		}
		else if (std::strcmp(componentName, "PointLight") == 0)
		{
			string strColor = conf.GetValue(section.c_str(), "color");
			string strPosition = conf.GetValue(section.c_str(), "position");

			auto& light = object.AddComponent<Components::PointLight>();
			light.color = Utils::StringToVec3f(strColor);
			light.diffuseIntensity = std::stof(conf.GetValue(section.c_str(), "diffuseIntensity"));
			light.specularIntensity = std::stof(conf.GetValue(section.c_str(), "specularIntensity"));
			light.position = Utils::StringToVec3f(strPosition);
		}
		else if (std::strcmp(componentName, "SpotLight") == 0)
		{
			string strColor = conf.GetValue(section.c_str(), "color");
			string strPosition = conf.GetValue(section.c_str(), "position");
			string strDirection = conf.GetValue(section.c_str(), "direction");

			auto& light = object.AddComponent<Components::SpotLight>();
			light.color = Utils::StringToVec3f(strColor);
			light.diffuseIntensity = std::stof(conf.GetValue(section.c_str(), "diffuseIntensity"));
			light.specularIntensity = std::stof(conf.GetValue(section.c_str(), "specularIntensity"));
			light.position = Utils::StringToVec3f(strPosition);
			light.direction = Utils::StringToVec3f(strDirection);
			light.attenuation.kl = std::stof(conf.GetValue(section.c_str(), "attenuation.kl"));
			light.attenuation.kq = std::stof(conf.GetValue(section.c_str(), "attenuation.kq"));
			light.cutOff = std::stof(conf.GetValue(section.c_str(), "cutOff"));
			light.outerCutOff = std::stof(conf.GetValue(section.c_str(), "outerCutOff"));
		}
	}
}

/* -----------------------------------------------------
 *          PRIVATE METHODS
 * -----------------------------------------------------
*/

#include "Scene.hpp"

#include "Core/GL.hpp"
#include "Core/Log/Logger.hpp"

#include "Engine/Utils.hpp"
#include "Engine/ECS/ECS.hpp"
#include "Engine/Filesystem/ConfigFile.hpp"
#include "Engine/Graphics/Shader.hpp"

/* ----------------------------------- */
/*								PUBLIC							 */
/* ------------------------------------*/

Scene::Scene(StringView filePath)
{
	LoadScene(filePath);
}
GameObject Scene::CreateObject(StringView label)
{
	GameObject object = GameObject(_registry.create(), &_registry);
	object.AddComponent<Label>(label.data());
	return object;
}
void Scene::DestroyObject(GameObject& object)
{
	_registry.destroy(object.GetObjectID());
}
void Scene::ClearScene()
{
	for (auto [entity, lComp] : _registry.view<Label>().each())
	{
		GameObject object{ entity, &_registry };
		
		/* Free GPU memory */
		if(auto* model = object.GetComponent<Model>())
			model->DestroyModel();
		if (auto* mesh = object.GetComponent<Mesh>())
			mesh->DestroyMesh();

		object.Invalidate();
	}
	
	/* Clear scene */
	_registry.clear();
}
void Scene::LoadScene(StringView filePath)
{
	CONSOLE_INFO("Loading scene {}...", filePath);
	DeserializeScene(filePath);
}
void Scene::SaveScene(StringView filePath)
{
	CONSOLE_INFO("Saving scene {}...", filePath);
	SerializeScene(filePath);
}

/* ----------------------------------- */
/*								PRIVATE							 */
/* ------------------------------------*/

void Scene::SerializeScene(StringView filePath)
{
	ConfigFile conf(filePath);
	for (auto [entity, label] : Reg().view<Label>().each())
	{
		GameObject object{ entity, &Reg() };
		const u32 objectID = static_cast<u32>(object.GetObjectID());

		String section = std::format("entity{}:Label", objectID);
		conf.Update(section, "label", label.value);

		if (auto* light = object.GetComponent<DirectionalLight>())
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
		if (auto* light = object.GetComponent<PointLight>())
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
		if (auto* light = object.GetComponent<SpotLight>())
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
		if (auto* transform = object.GetComponent<Transform>())
		{
			String section = std::format("entity{}:Transform", objectID);
			conf.Update(section, "position", std::format("{},{},{}", transform->position.x, transform->position.y, transform->position.z));
			conf.Update(section, "scale", std::format("{},{},{}", transform->scale.x, transform->scale.y, transform->scale.z));
			conf.Update(section, "rotation", std::format("{},{},{}", transform->rotation.x, transform->rotation.y, transform->rotation.z));
		}
		if (auto* model = object.GetComponent<Model>())
		{
			String section = std::format("entity{}:Model", objectID);
			conf.Update(section, "path", model->strPath);
		}
	}
	conf.Generate(true);
}
void Scene::DeserializeScene(StringView filePath)
{
	ConfigFile conf(filePath);
	conf.ReadData();

	GameObject object;
	u32 currentObjectId = static_cast<u32>(entt::null);
	for (const auto& it : conf.GetData())
	{
		const String& section = it.first;
		StringView view{ section };
		i32 dots = view.find_first_of(':');
		u32 objectId = std::strtoul(view.substr(6, (dots - 6)).data(), nullptr, 10);
		StringView component = view.substr(dots + 1);

		if (currentObjectId != objectId)
		{
			currentObjectId = objectId;
			object = CreateObject();
		}

		if (component == "Label")
		{
			const String& labelVal = conf.GetValue(section, "label");
			auto* label = object.GetComponent<Label>();
			label->UpdateValue(labelVal);
		}
		else if (component == "Transform")
		{
			const String& position = conf.GetValue(section, "position");
			const String& rotation = conf.GetValue(section, "rotation");
			const String& scale = conf.GetValue(section, "scale");

			auto& transform = object.AddComponent<Transform>();
			transform.position = Utils::StringToVec3f(position);
			transform.rotation = Utils::StringToVec3f(rotation);
			transform.scale = Utils::StringToVec3f(scale);
			transform.UpdateTransformation();
		}
		else if (component == "Model")
		{
			const String& strPath = conf.GetValue(section, "path");
			object.AddComponent<Model>(strPath);
		}
		else if (component == "DirectionalLight")
		{
			const String& color = conf.GetValue(section, "color");
			const String& diffuseIntensity = conf.GetValue(section, "diffuseIntensity");
			const String& specularIntensity = conf.GetValue(section, "specularIntensity");
			const String& direction = conf.GetValue(section, "direction");

			auto& light = object.AddComponent<DirectionalLight>();
			light.color = Utils::StringToVec3f(color);
			light.diffuseIntensity = std::stof(diffuseIntensity);
			light.specularIntensity = std::stof(specularIntensity);
			light.direction = Utils::StringToVec3f(direction);
		}
		else if (component == "PointLight")
		{
			const String& color = conf.GetValue(section, "color");
			const String& diffuseIntensity = conf.GetValue(section, "diffuseIntensity");
			const String& specularIntensity = conf.GetValue(section, "specularIntensity");
			const String& position = conf.GetValue(section, "position");
			const String& kl = conf.GetValue(section, "attenuation.kl");
			const String& kq = conf.GetValue(section, "attenuation.kq");

			auto& light = object.AddComponent<PointLight>();
			light.color = Utils::StringToVec3f(color);
			light.diffuseIntensity = std::stof(diffuseIntensity);
			light.specularIntensity = std::stof(specularIntensity);
			light.position = Utils::StringToVec3f(position);
			light.attenuation.kl = std::stof(kl);
			light.attenuation.kq = std::stof(kq);
		}
		else if (component == "SpotLight")
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

			auto& light = object.AddComponent<SpotLight>();
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

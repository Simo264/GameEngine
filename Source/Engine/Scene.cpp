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
GameObject Scene::CreateObject()
{
	auto id = _registry.create();
	
	char defaultTag[64]{};
	std::format_to_n(defaultTag, sizeof(defaultTag), "Object_{}", static_cast<u32>(id));

	GameObject object = GameObject{ id, &_registry };
	object.AddComponent<Tag>(defaultTag);
	return object;
}
void Scene::DestroyObject(GameObject& object)
{
	_registry.destroy(object.ID());
}
void Scene::ClearScene()
{
	for (auto [entity, lComp] : _registry.view<Tag>().each())
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
	for (auto [entity, tag] : Reg().view<Tag>().each())
	{
		GameObject object{ entity, &Reg() };
		const u32 objectID = static_cast<u32>(object.ID());

		String section = std::format("Entity{}:Tag", objectID);
		conf.Update(section, "value", tag.value);

		if (auto* transform = object.GetComponent<Transform>())
		{
			String section = std::format("Entity{}:Transform", objectID);
			conf.Update(section, "position", std::format("{},{},{}", transform->position.x, transform->position.y, transform->position.z));
			conf.Update(section, "scale", std::format("{},{},{}", transform->scale.x, transform->scale.y, transform->scale.z));
			conf.Update(section, "rotation", std::format("{},{},{}", transform->rotation.x, transform->rotation.y, transform->rotation.z));
		}
		if (auto* model = object.GetComponent<Model>())
		{
			String section = std::format("Entity{}:Model", objectID);
			conf.Update(section, "path", model->strPath);
		}
		if (auto* light = object.GetComponent<Light>())
		{
			String section = std::format("Entity{}:Light", objectID);
			String type = std::to_string(static_cast<i32>(light->type));
			conf.Update(section, "type", type);
			switch (light->type)
			{
			case LightType::DIRECTIONAL:
			{
				auto* dirLight = object.GetComponent<DirectionalLight>();
				String color = std::format("{},{},{}", dirLight->color.x, dirLight->color.y, dirLight->color.z);
				String diffuseIntensity = std::to_string(dirLight->diffuseIntensity);
				String specularIntensity = std::to_string(dirLight->specularIntensity);
				String direction = std::format("{},{},{}", dirLight->direction.x, dirLight->direction.y, dirLight->direction.z);
				conf.Update(section, "color", color);
				conf.Update(section, "diffuseIntensity", diffuseIntensity);
				conf.Update(section, "specularIntensity", specularIntensity);
				conf.Update(section, "direction", direction);
				break;
			}
			case LightType::POINT:
			{
				auto* pointLight = object.GetComponent<PointLight>();
				String color = std::format("{},{},{}", pointLight->color.x, pointLight->color.y, pointLight->color.z);
				String diffuseIntensity = std::to_string(pointLight->diffuseIntensity);
				String specularIntensity = std::to_string(pointLight->specularIntensity);
				String position = std::format("{},{},{}", pointLight->position.x, pointLight->position.y, pointLight->position.z);
				String range = std::to_string(pointLight->attenuation.range);
				String kl = std::to_string(pointLight->attenuation.kl);
				String kq = std::to_string(pointLight->attenuation.kq);
				conf.Update(section, "color", color);
				conf.Update(section, "diffuseIntensity", diffuseIntensity);
				conf.Update(section, "specularIntensity", specularIntensity);
				conf.Update(section, "position", position);
				conf.Update(section, "attenuation.range", range);
				conf.Update(section, "attenuation.kl", kl);
				conf.Update(section, "attenuation.kq", kq);
				break;
			}
			case LightType::SPOT:
			{
				auto* spotLight = object.GetComponent<SpotLight>();
				String color = std::format("{},{},{}", spotLight->color.x, spotLight->color.y, spotLight->color.z);
				String diffuseIntensity = std::to_string(spotLight->diffuseIntensity);
				String specularIntensity = std::to_string(spotLight->specularIntensity);
				String direction = std::format("{},{},{}", spotLight->direction.x, spotLight->direction.y, spotLight->direction.z);
				String position = std::format("{},{},{}", spotLight->position.x, spotLight->position.y, spotLight->position.z);
				String cutOff = std::to_string(spotLight->cutOff);
				String outerCutOff = std::to_string(spotLight->outerCutOff);
				String range = std::to_string(spotLight->attenuation.range);
				String kl = std::to_string(spotLight->attenuation.kl);
				String kq = std::to_string(spotLight->attenuation.kq);
				conf.Update(section, "color", color);
				conf.Update(section, "diffuseIntensity", diffuseIntensity);
				conf.Update(section, "specularIntensity", specularIntensity);
				conf.Update(section, "direction", direction);
				conf.Update(section, "position", position);
				conf.Update(section, "attenuation.range", range);
				conf.Update(section, "attenuation.kl", kl);
				conf.Update(section, "attenuation.kq", kq);
				conf.Update(section, "cutOff", cutOff);
				conf.Update(section, "outerCutOff", outerCutOff);
				break;
			}
			}
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

		if (component == "Tag")
		{
			const String& value = conf.GetValue(section, "value");
			object.GetComponent<Tag>()->UpdateValue(value);
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
		else if (component == "Light")
		{
			i32 type = Utils::StringToI32(conf.GetValue(section, "type"));
			object.AddComponent<Light>(static_cast<LightType>(type));

			switch (static_cast<LightType>(type))
			{
			case LightType::DIRECTIONAL:
			{
				const String& color = conf.GetValue(section, "color");
				const String& diffuseIntensity = conf.GetValue(section, "diffuseIntensity");
				const String& specularIntensity = conf.GetValue(section, "specularIntensity");
				const String& direction = conf.GetValue(section, "direction");
				auto& light = object.AddComponent<DirectionalLight>();
				light.color = Utils::StringToVec3f(color);
				light.diffuseIntensity = Utils::StringToF32(diffuseIntensity);
				light.specularIntensity = Utils::StringToF32(specularIntensity);
				light.direction = Utils::StringToVec3f(direction);
				break;
			}
			case LightType::POINT:
			{
				const String& color = conf.GetValue(section, "color");
				const String& diffuseIntensity = conf.GetValue(section, "diffuseIntensity");
				const String& specularIntensity = conf.GetValue(section, "specularIntensity");
				const String& position = conf.GetValue(section, "position");
				const String& range = conf.GetValue(section, "attenuation.range");
				const String& kl = conf.GetValue(section, "attenuation.kl");
				const String& kq = conf.GetValue(section, "attenuation.kq");
				auto& light = object.AddComponent<PointLight>();
				light.color = Utils::StringToVec3f(color);
				light.diffuseIntensity = Utils::StringToF32(diffuseIntensity);
				light.specularIntensity = Utils::StringToF32(specularIntensity);
				light.position = Utils::StringToVec3f(position);
				light.attenuation.range = Utils::StringToI32(range);
				light.attenuation.kl = Utils::StringToF32(kl);
				light.attenuation.kq = Utils::StringToF32(kq);
				break;
			}
			case LightType::SPOT:
			{
				const String& color = conf.GetValue(section, "color");
				const String& diffuseIntensity = conf.GetValue(section, "diffuseIntensity");
				const String& specularIntensity = conf.GetValue(section, "specularIntensity");
				const String& position = conf.GetValue(section, "position");
				const String& direction = conf.GetValue(section, "direction");
				const String& range = conf.GetValue(section, "attenuation.range");
				const String& kl = conf.GetValue(section, "attenuation.kl");
				const String& kq = conf.GetValue(section, "attenuation.kq");
				const String& cutOff = conf.GetValue(section, "cutOff");
				const String& outerCutOff = conf.GetValue(section, "outerCutOff");
				auto& light = object.AddComponent<SpotLight>();
				light.color = Utils::StringToVec3f(color);
				light.diffuseIntensity = Utils::StringToF32(diffuseIntensity);
				light.specularIntensity = Utils::StringToF32(specularIntensity);
				light.position = Utils::StringToVec3f(position);
				light.direction = Utils::StringToVec3f(direction);
				light.attenuation.range = Utils::StringToI32(range);
				light.attenuation.kl = Utils::StringToF32(kl);
				light.attenuation.kq = Utils::StringToF32(kq);
				light.cutOff = Utils::StringToF32(cutOff);
				light.outerCutOff = Utils::StringToF32(outerCutOff);
				break;
			}

			default:
				throw std::runtime_error("Invalid LightType!");
			}
		}
	}
}

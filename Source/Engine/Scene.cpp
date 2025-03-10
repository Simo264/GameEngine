#include "Scene.hpp"

#include "Core/GL.hpp"
#include "Core/Log/Logger.hpp"

#include "Engine/Utils.hpp"
#include "Engine/ECS/ECS.hpp"
#include "Engine/IniFileHandler.hpp"
#include "Engine/Graphics/Shader.hpp"
#include "Engine/Subsystems/ModelsManager.hpp"
#include "Engine/Subsystems/AnimationsManager.hpp"
#include "Engine/Filesystem/Filesystem.hpp"

// ----------------------------------- 
//								PUBLIC							 
// -----------------------------------

Scene::Scene(const fs::path& filePath)
{
	LoadFromFile(filePath);
}
GameObject Scene::CreateObject(StringView objName)
{
	entt::entity id = _registry.create();
	
	char defaultTag[32]{};
	if (objName.empty())
		std::format_to_n(defaultTag, sizeof(defaultTag), "Object_{}", static_cast<u32>(id));
	else
		std::strncpy(defaultTag, objName.data(), sizeof(defaultTag));

	GameObject object{ id, &_registry };
	object.AddComponent<Tag>(defaultTag);
	return object;
}
void Scene::DestroyObject(entt::entity id)
{
	if (_registry.valid(id))
		_registry.destroy(id);
	else
		CONSOLE_WARN("Entity id {} is not a valid object", static_cast<u32>(id));
}
void Scene::Clear()
{
	_registry.clear();
}
void Scene::LoadFromFile(const fs::path& filePath)
{
	CONSOLE_INFO("Loading scene {}...", filePath.relative_path().string());
	DeserializeScene(filePath);
}
void Scene::Save(const fs::path& filePath)
{
	CONSOLE_INFO("Saving scene {}...", filePath.relative_path().string());
	SerializeScene(filePath);
}

// -----------------------------------
//								PRIVATE							
// -----------------------------------

void Scene::SerializeScene(const fs::path& filePath)
{
	IniFileHandler conf(filePath);
	for (auto [entity, tag] : Reg().view<Tag>().each())
	{
		GameObject object{ entity, &Reg() };
		const u32 objectID = static_cast<u32>(object.id);

		String section = std::format("Entity{}:Tag", objectID);
		conf.Update(section, "value", tag.value.data());

		if (Transform* transform = object.GetComponent<Transform>())
		{
			String section = std::format("Entity{}:Transform", objectID);
			conf.Update(section, "position", std::format("{},{},{}", transform->position.x, transform->position.y, transform->position.z));
			conf.Update(section, "scale", std::format("{},{},{}", transform->scale.x, transform->scale.y, transform->scale.z));
			conf.Update(section, "rotation", std::format("{},{},{}", transform->rotation.x, transform->rotation.y, transform->rotation.z));
		}
		if (StaticMesh* staticMesh = object.GetComponent<StaticMesh>())
		{
			auto& manager = ModelsManager::Get();
			String section = std::format("Entity{}:StaticMesh", objectID);
			const fs::path* path = manager.GetStaticMeshPath(staticMesh->id);
			conf.Update(section, "path", path->string());
		}
		if (SkeletalMesh* skeleton = object.GetComponent<SkeletalMesh>())
		{
			auto& manager = ModelsManager::Get();
			String section = std::format("Entity{}:SkeletalMesh", objectID);
			const fs::path* path = manager.GetSkeletalMeshPath(skeleton->id);
			conf.Update(section, "path", path->string());
		}
		if (Light* light = object.GetComponent<Light>())
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
				String intensity = std::to_string(dirLight->intensity);
				String direction = std::format("{},{},{}", dirLight->direction.x, dirLight->direction.y, dirLight->direction.z);
				conf.Update(section, "color", color);
				conf.Update(section, "intensity", intensity);
				conf.Update(section, "direction", direction);
				break;
			}
			case LightType::POINT:
			{
				auto* pointLight = object.GetComponent<PointLight>();
				String color = std::format("{},{},{}", pointLight->color.x, pointLight->color.y, pointLight->color.z);
				String intensity = std::to_string(pointLight->intensity);
				String position = std::format("{},{},{}", pointLight->position.x, pointLight->position.y, pointLight->position.z);
				String range = std::to_string(pointLight->attenuation.range);
				String kl = std::to_string(pointLight->attenuation.kl);
				String kq = std::to_string(pointLight->attenuation.kq);
				conf.Update(section, "color", color);
				conf.Update(section, "intensity", intensity);
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
				String intensity = std::to_string(spotLight->intensity);
				String direction = std::format("{},{},{}", spotLight->direction.x, spotLight->direction.y, spotLight->direction.z);
				String position = std::format("{},{},{}", spotLight->position.x, spotLight->position.y, spotLight->position.z);
				String cutOff = std::to_string(spotLight->cutOff);
				String outerCutOff = std::to_string(spotLight->outerCutOff);
				String range = std::to_string(spotLight->attenuation.range);
				String kl = std::to_string(spotLight->attenuation.kl);
				String kq = std::to_string(spotLight->attenuation.kq);
				conf.Update(section, "color", color);
				conf.Update(section, "intensity", intensity);
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
	conf.Generate(false);
}
void Scene::DeserializeScene(const fs::path& filePath)
{
	IniFileHandler conf(filePath);
	conf.ReadData();

	GameObject object;
	u32 currentObjectId = static_cast<u32>(entt::null);
	for (const auto& it : conf.GetData())
	{
		const String& section = it.first;
		StringView view{ section };
		u32 dots = view.find_first_of(':');
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
		else if (component == "StaticMesh")
		{
			auto& manager = ModelsManager::Get();
			const String& strPath = conf.GetValue(section, "path");
			const StaticMesh* staticMesh = manager.FindStaticMesh(strPath);
			if (!staticMesh)
				staticMesh = &manager.CreateStaticMesh(strPath);

			StaticMesh& component = object.AddComponent<StaticMesh>();
			staticMesh->Clone(component);
		}
		else if (component == "SkeletalMesh")
		{
			SkeletalMesh& skmeshComponent = object.AddComponent<SkeletalMesh>();
			Animator& animatorComponent = object.AddComponent<Animator>();

			ModelsManager& modManager = ModelsManager::Get();
			AnimationsManager& animManager = AnimationsManager::Get();

			// The relative path to "Assets/Models/Skeletal" 
			// E.g. "Mutant/Mutant.gltf"
			fs::path relative = conf.GetValue(section, "path");
			const SkeletalMesh* skeleton = modManager.FindSkeletalMesh(relative);
			if (!skeleton)
			{
				skeleton = &modManager.CreateSkeletalMesh(relative);
				
				// E.g. skeletonPath = "Mutant/"
				fs::path parent = relative.parent_path();
				// E.g. skeletonPath = "D:\GameEngine\Assets\Models\Skeletal\Mutant"
				fs::path absolute = (Filesystem::GetSkeletalModelsPath() / parent);
				// E.g. skeletonPath = "D:\GameEngine\Assets\Models\Skeletal\animlist.txt"
				fs::path animlistFile = absolute / "animlist.txt";

				Vector<fs::path> relativeAnims{};
				if (!fs::exists(animlistFile))
				{
					CONSOLE_WARN("{} file does not exist");
				}
				else
				{
					IStream file(animlistFile);

					// E.g. relativeAnims = [ 
					//	"Drunk_Walk/<filename>.gltf", 
					//	"Silly_Dancing/<filename>.gltf" 
					// ]
					relativeAnims.assign(std::istream_iterator<fs::path>(file), std::istream_iterator<fs::path>());

					// E.g. relativeAnims = [ 
					//	"Mutant/Drunk_Walk/<filename>.gltf", 
					//	"Mutant/Silly_Dancing/<filename>.gltf" 
					// ]
					for (auto& p : relativeAnims)
						p = parent / p;
				}

				animManager.LoadAnimations(*skeleton, relativeAnims);
			}
			
			skeleton->Clone(skmeshComponent);
			animatorComponent.SetTargetSkeleton(skmeshComponent);
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
				const String& intensity = conf.GetValue(section, "intensity");
				const String& direction = conf.GetValue(section, "direction");
				auto& light = object.AddComponent<DirectionalLight>();
				light.color = Utils::StringToVec3f(color);
				light.intensity = Utils::StringToF32(intensity);
				light.direction = Utils::StringToVec3f(direction);
				break;
			}
			case LightType::POINT:
			{
				const String& color = conf.GetValue(section, "color");
				const String& intensity = conf.GetValue(section, "intensity");
				const String& position = conf.GetValue(section, "position");
				const String& range = conf.GetValue(section, "attenuation.range");
				const String& kl = conf.GetValue(section, "attenuation.kl");
				const String& kq = conf.GetValue(section, "attenuation.kq");
				auto& light = object.AddComponent<PointLight>();
				light.color = Utils::StringToVec3f(color);
				light.intensity = Utils::StringToF32(intensity);
				light.position = Utils::StringToVec3f(position);
				light.attenuation.range = Utils::StringToI32(range);
				light.attenuation.kl = Utils::StringToF32(kl);
				light.attenuation.kq = Utils::StringToF32(kq);
				break;
			}
			case LightType::SPOT:
			{
				const String& color = conf.GetValue(section, "color");
				const String& intensity = conf.GetValue(section, "intensity");
				const String& position = conf.GetValue(section, "position");
				const String& direction = conf.GetValue(section, "direction");
				const String& range = conf.GetValue(section, "attenuation.range");
				const String& kl = conf.GetValue(section, "attenuation.kl");
				const String& kq = conf.GetValue(section, "attenuation.kq");
				const String& cutOff = conf.GetValue(section, "cutOff");
				const String& outerCutOff = conf.GetValue(section, "outerCutOff");
				auto& light = object.AddComponent<SpotLight>();
				light.color = Utils::StringToVec3f(color);
				light.intensity = Utils::StringToF32(intensity);
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
				throw std::runtime_error("error on loading scene: invalid LightType");
			}
		}
	}
}

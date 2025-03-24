#include "Scene.hpp"

#include "Core/OpenGL.hpp"
#include "Core/Log/Logger.hpp"
#include "Core/Paths/Paths.hpp"
#include "Core/Serialization/YAMLParser.hpp"

#include "Engine/Utils.hpp"
#include "Engine/ECS/ECS.hpp"
#include "Engine/Graphics/Shader.hpp"
#include "Engine/Subsystems/ModelsManager.hpp"
#include "Engine/Subsystems/AnimationsManager.hpp"

// ----------------------------------- 
//								PUBLIC							 
// -----------------------------------

Scene::Scene(const fs::path& loadFrom)
{
	LoadFromFile(loadFrom);
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
void Scene::LoadFromFile(const fs::path& loadFrom)
{
	CONSOLE_INFO("Loading scene {}...", loadFrom.relative_path().string());
	DeserializeScene(loadFrom);
}
void Scene::SaveToFile(const fs::path& out)
{
	CONSOLE_INFO("Saving scene {}...", out.relative_path().string());
	SerializeScene(out);
}

// -----------------------------------
//								PRIVATE							
// -----------------------------------

void Scene::SerializeScene(const fs::path& out)
{
	ModelsManager& modelsManager = ModelsManager::Get();
	
	YAML::Emitter outEmitter;
	outEmitter << YAML::BeginMap;
	for (auto entity : Reg().view<entt::entity>())
	{
		GameObject object{ entity, &Reg() };

		Array<char, 32> entityName{};
		std::format_to_n(
			entityName.data(),
			entityName.size(),
			"Entity{}", static_cast<u32>(object.id));

		outEmitter << YAML::Key << entityName.data();
		outEmitter << YAML::BeginMap;
		
		Tag& tag = *object.GetComponent<Tag>();
		outEmitter << YAML::Key << "Tag" << YAML::Value << tag.value.data();

		if(Transform* transform = object.GetComponent<Transform>())
		{ 
			outEmitter << YAML::Key << "Transform";
			outEmitter << YAML::BeginMap;
			outEmitter << YAML::Key << "position"; 
			outEmitter << YAML::Flow << YAML::BeginSeq;
			outEmitter << transform->position.x << transform->position.y << transform->position.z; 
			outEmitter << YAML::EndSeq;
			outEmitter << YAML::Key << "scale";
			outEmitter << YAML::Flow << YAML::BeginSeq;
			outEmitter << transform->scale.x << transform->scale.y << transform->scale.z;
			outEmitter << YAML::EndSeq;
			outEmitter << YAML::Key << "rotation"; 
			outEmitter << YAML::Flow << YAML::BeginSeq;
			outEmitter << transform->rotation.x << transform->rotation.y << transform->rotation.z;
			outEmitter << YAML::EndSeq;
			outEmitter << YAML::EndMap;
		}
		if (StaticMesh* staticMesh = object.GetComponent<StaticMesh>())
		{
			const fs::path& path = *modelsManager.GetStaticMeshPath(staticMesh->id);
			outEmitter << YAML::Key << "StaticMesh";
			outEmitter << YAML::BeginMap;
			outEmitter << YAML::Key << "path" << YAML::Value << path.string();
			outEmitter << YAML::EndMap;
		}
		if (SkeletalMesh* skeleton = object.GetComponent<SkeletalMesh>())
		{
			const fs::path& path = *modelsManager.GetSkeletalMeshPath(skeleton->id);
			outEmitter << YAML::Key << "SkeletalMesh";
			outEmitter << YAML::BeginMap;
			outEmitter << YAML::Key << "path" << YAML::Value << path.string();
			outEmitter << YAML::EndMap;

		}
		if (Light* light = object.GetComponent<Light>())
		{
			outEmitter << YAML::Key << "Light";
			outEmitter << YAML::BeginMap;
			outEmitter << YAML::Key << "type" << YAML::Value << static_cast<i32>(light->type);
			switch (light->type)
			{
				case LightType::DIRECTIONAL:
				{
					auto& dirLight = *object.GetComponent<DirectionalLight>();
					outEmitter << YAML::Key << "color";
					outEmitter << YAML::Flow << YAML::BeginSeq;
					outEmitter << dirLight.color.x << dirLight.color.y << dirLight.color.z;
					outEmitter << YAML::EndSeq;
					outEmitter << YAML::Key << "direction";
					outEmitter << YAML::Flow << YAML::BeginSeq;
					outEmitter << dirLight.direction.x << dirLight.direction.y << dirLight.direction.z;
					outEmitter << YAML::EndSeq;
					outEmitter << YAML::Key << "intensity" << YAML::Value << dirLight.intensity;
					break;
				}
				case LightType::POINT:
				{
					auto& pointLight = *object.GetComponent<PointLight>();
					outEmitter << YAML::Key << "color";
					outEmitter << YAML::Flow << YAML::BeginSeq;
					outEmitter << pointLight.color.x << pointLight.color.y << pointLight.color.z;
					outEmitter << YAML::EndSeq;
					outEmitter << YAML::Key << "position";
					outEmitter << YAML::Flow << YAML::BeginSeq;
					outEmitter << pointLight.position.x << pointLight.position.y << pointLight.position.z;
					outEmitter << YAML::EndSeq;
					outEmitter << YAML::Key << "intensity" << YAML::Value << pointLight.intensity;
					outEmitter << YAML::Key << "attenuation.range" << YAML::Value << pointLight.attenuation.range;
					outEmitter << YAML::Key << "attenuation.kl" << YAML::Value << pointLight.attenuation.kl;
					outEmitter << YAML::Key << "attenuation.kq" << YAML::Value << pointLight.attenuation.kq;
					break;
				}
				case LightType::SPOT:
				{
					auto& spotLight = *object.GetComponent<SpotLight>();
					outEmitter << YAML::Key << "color";
					outEmitter << YAML::Flow << YAML::BeginSeq;
					outEmitter << spotLight.color.x << spotLight.color.y << spotLight.color.z;
					outEmitter << YAML::EndSeq;
					outEmitter << YAML::Key << "direction";
					outEmitter << YAML::Flow << YAML::BeginSeq;
					outEmitter << spotLight.direction.x << spotLight.direction.y << spotLight.direction.z;
					outEmitter << YAML::EndSeq;
					outEmitter << YAML::Key << "position";
					outEmitter << YAML::Flow << YAML::BeginSeq;
					outEmitter << spotLight.position.x << spotLight.position.y << spotLight.position.z;
					outEmitter << YAML::EndSeq;
					outEmitter << YAML::Key << "intensity" << YAML::Value << spotLight.intensity;
					outEmitter << YAML::Key << "attenuation.range" << YAML::Value << spotLight.attenuation.range;
					outEmitter << YAML::Key << "attenuation.kl" << YAML::Value << spotLight.attenuation.kl;
					outEmitter << YAML::Key << "attenuation.kq" << YAML::Value << spotLight.attenuation.kq;
					outEmitter << YAML::Key << "cutOff" << YAML::Value << spotLight.cutOff;
					outEmitter << YAML::Key << "outerCutOff" << YAML::Value << spotLight.outerCutOff;
					break;
				}
			}
			
			outEmitter << YAML::EndMap;
		}

		outEmitter << YAML::EndMap;
	}
	
	outEmitter << YAML::EndMap;
	OutputFileStream outFile(out);
	outFile << outEmitter.c_str();
	outFile.close();
}
void Scene::DeserializeScene(const fs::path& loadFrom)
{
	ModelsManager& modelsManager = ModelsManager::Get();
	AnimationsManager& animatorManager = AnimationsManager::Get();

	YAML::Node scene = YAML::LoadFile(loadFrom.string());
	for (const auto& entity : scene)
	{
		GameObject object = CreateObject();

		// Itera attraverso i componenti dell'entità
		for (const auto& component : entity.second)
		{
			String componentName = component.first.as<String>();
			if (componentName == "Tag")
			{
				String tag = component.second.as<String>();
				object.GetComponent<Tag>()->UpdateValue(tag);
			}
			else if (componentName == "Transform")
			{
				Transform& transform = object.AddComponent<Transform>();
				YAML::Node node = component.second["position"];
				transform.position = {
					node[0].as<f32>(),
					node[1].as<f32>(),
					node[2].as<f32>()
				}; 
				node = component.second["scale"];
				transform.scale = {
					node[0].as<f32>(),
					node[1].as<f32>(),
					node[2].as<f32>()
				};
				node = component.second["rotation"];
				transform.rotation = {
					node[0].as<f32>(),
					node[1].as<f32>(),
					node[2].as<f32>()
				};
				transform.UpdateTransformation();
			}
			else if (componentName == "StaticMesh")
			{
				// The relative path to "Assets/Models/Static" (e.g. "cube/cube.obj")
				fs::path path = component.second["path"].as<String>();
				const StaticMesh* staticMesh = modelsManager.FindStaticMesh(path);
				if (!staticMesh)
					staticMesh = &modelsManager.CreateStaticMesh(path);

				StaticMesh& staticMeshComponent = object.AddComponent<StaticMesh>();
				staticMesh->Clone(staticMeshComponent);
			}
			else if (componentName == "SkeletalMesh")
			{
				SkeletalMesh& skeletalMeshComponent = object.AddComponent<SkeletalMesh>();
				Animator& animatorComponent = object.AddComponent<Animator>();

				// The relative path to "Assets/Models/Skeletal" (e.g. "Mutant/Mutant.gltf")
				fs::path path = component.second["path"].as<String>();
				const SkeletalMesh* skeleton = modelsManager.FindSkeletalMesh(path);
				if (!skeleton)
				{
					skeleton = &modelsManager.CreateSkeletalMesh(path);

					// skeletonPath = "Mutant/"
					fs::path parent = path.parent_path();
					// skeletonPath = "GameEngine/Assets/Models/Skeletal/Mutant"
					fs::path absolute = (Paths::GetSkeletalModelsPath() / parent);
					// skeletonPath = "GameEngine/Assets/Models/Skeletal/animlist.txt"
					fs::path animlistFile = absolute / "animlist.txt";
					
					Vector<fs::path> relativeAnims{};
					if (!fs::exists(animlistFile))
					{
						CONSOLE_WARN("{} file does not exist");
					}
					else
					{
						InputFileStream file(animlistFile);
						// relativeAnims = [ 
						//	"Drunk_Walk/<filename>.gltf", 
						//	"Silly_Dancing/<filename>.gltf" 
						// ]
						relativeAnims.assign(
							std::istream_iterator<fs::path>(file), 
							std::istream_iterator<fs::path>());

						// relativeAnims = [ 
						//	"Mutant/Drunk_Walk/<filename>.gltf", 
						//	"Mutant/Silly_Dancing/<filename>.gltf" 
						// ]
						for (auto& p : relativeAnims)
							p = parent / p;
					}
					animatorManager.LoadAnimations(*skeleton, relativeAnims);
				}
				skeleton->Clone(skeletalMeshComponent);
				animatorComponent.SetTargetSkeleton(skeletalMeshComponent);
			}
			else if (componentName == "Light")
			{
				i32 type = component.second["type"].as<i32>();
				object.AddComponent<Light>(static_cast<LightType>(type));
				switch (static_cast<LightType>(type))
				{
					case LightType::DIRECTIONAL:
					{
						auto& light = object.AddComponent<DirectionalLight>();
						YAML::Node node = component.second["color"];
						light.color = {
							node[0].as<f32>(),
							node[1].as<f32>(),
							node[2].as<f32>()
						};
						node = component.second["direction"];
						light.direction = {
							node[0].as<f32>(),
							node[1].as<f32>(),
							node[2].as<f32>()
						};
						light.intensity = component.second["intensity"].as<f32>();
						break;
					}
					case LightType::POINT:
					{
						auto& light = object.AddComponent<PointLight>();
						YAML::Node node = component.second["color"];
						light.color = {
							node[0].as<f32>(),
							node[1].as<f32>(),
							node[2].as<f32>()
						};
						node = component.second["position"];
						light.position = {
							node[0].as<f32>(),
							node[1].as<f32>(),
							node[2].as<f32>()
						};
						light.intensity = component.second["intensity"].as<f32>();
						light.attenuation.range = component.second["attenuation.range"].as<i32>();
						light.attenuation.kl = component.second["attenuation.kl"].as<f32>();
						light.attenuation.kq = component.second["attenuation.kq"].as<f32>();
						break;
					}
					case LightType::SPOT:
					{
						auto& light = object.AddComponent<SpotLight>();
						YAML::Node node = component.second["color"];
						light.color = {
							node[0].as<f32>(),
							node[1].as<f32>(),
							node[2].as<f32>()
						};
						node = component.second["direction"];
						light.direction = {
							node[0].as<f32>(),
							node[1].as<f32>(),
							node[2].as<f32>()
						};
						node = component.second["position"];
						light.position = {
							node[0].as<f32>(),
							node[1].as<f32>(),
							node[2].as<f32>()
						};
						light.intensity = component.second["intensity"].as<f32>();
						light.attenuation.range = component.second["attenuation.range"].as<i32>();
						light.attenuation.kl = component.second["attenuation.kl"].as<f32>();
						light.attenuation.kq = component.second["attenuation.kq"].as<f32>();
						light.cutOff = component.second["cutOff"].as<f32>();
						light.outerCutOff = component.second["outerCutOff"].as<f32>();
						break;
					}
					default:
						throw std::runtime_error("invalid LightType");
				}
			}
		}
	}
}

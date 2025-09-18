#include "SceneSerializer.hpp"

#include "Core/Log/Logger.hpp"
#include "Core/Paths/Paths.hpp"

#include "Scene.hpp"
#include "Managers/StaticMeshFactory.hpp"

using namespace Components;

enum class ComponentType
{
	Archetype, Tag, Transformation, StaticMesh, Light, Unknown
};
static ComponentType GetComponentType(StringView name)
{
	if (name.compare("tag") == 0) 
		return ComponentType::Tag;
	if (name.compare("transformation") == 0) 
		return ComponentType::Transformation;
	if (name.compare("staticmesh") == 0) 
		return ComponentType::StaticMesh;
	if (name.compare("light") == 0) 
		return ComponentType::Light;
	
	return ComponentType::Unknown;
}

void SceneSerializer::SerializeScene(Scene& scene, const fs::path& outPath)
{
	auto outEmitter = YAML::Emitter{};
	outEmitter << YAML::BeginMap;
	for (auto entity : scene.GetEntityRegistry().view<ArchetypeComponent>())
	{
		auto& archetypeComp = scene.GetEntityRegistry().get<ArchetypeComponent>(entity);
		auto archetypeId = archetypeComp.archetypeId;
		auto archetype = scene.GetArchetypeRegistry().GetArchetype(archetypeId);
		auto object = GameObject{ entity, &scene.GetEntityRegistry(), archetype };

		auto entityName = Array<char, 32>{};
		std::format_to(entityName.data(), "Entity{}", static_cast<u32>(object.GetID()));

		outEmitter << YAML::Key << entityName.data();
		outEmitter << YAML::BeginMap;

		outEmitter << YAML::Key << "Archetype";
		outEmitter << YAML::Value << (archetype ? archetype->GetName() : "Unknown");

		auto tag = *object.GetComponent<Tag>();
		__SerializeTag(outEmitter, tag);

		if (auto transform = object.GetComponent<Transform>())
			__SerializeTransformation(outEmitter, *transform);

		if (auto staticMesh = object.GetComponent<StaticMesh>())
			__SerializeStaticMesh(outEmitter, *staticMesh);

		if (auto light = object.GetComponent<Light>())
			__SerializeLight(outEmitter, *light, object);

		outEmitter << YAML::EndMap;
	}

	outEmitter << YAML::EndMap;
	auto outFile = OutputFileStream(outPath);
	outFile << outEmitter.c_str();
	outFile.close();
}
void SceneSerializer::DeserializeScene(Scene& scene, const fs::path& fromPath)
{
	if (!fs::exists(fromPath))
		throw std::runtime_error(std::format("{} does not exist!", fromPath.string()));

	auto yamlScene = YAML::LoadFile(fromPath.string());
	for (const auto& entity : yamlScene)
	{
		if (!entity.second["Archetype"])
		{
			CONSOLE_ERROR("Missing Archetype field for entity: {}", entity.first.as<String>());
			continue;
		}
		auto archetypeName = entity.second["Archetype"].as<String>();
		auto archetypeId = scene.GetArchetypeRegistry().GetArchetypeId(archetypeName);
		if (archetypeId == INVALID_ARCHETYPE_ID)
		{
			CONSOLE_ERROR("Unknown archetype: {} for entity: {}", archetypeName, entity.first.as<String>());
			continue;
		}

		auto object = scene.CreateObject(archetypeId);

		for (const auto& component : entity.second)
		{
			auto componentName = component.first.as<String>();
			std::transform(componentName.begin(), 
										 componentName.end(), 
										 componentName.begin(),[](char c) { return std::tolower(c); });

			auto& node = component.second;
			switch (GetComponentType(componentName))
			{
				case ComponentType::Archetype: // do nothing
					break;
				case ComponentType::Tag:
					__DeserializeTag(object, node);
					break;
				case ComponentType::Transformation:
					__DeserializeTransformation(object, node); 
					break;
				case ComponentType::StaticMesh:    
					__DeserializeStaticMesh(object, node); 
					break;
				case ComponentType::Light:         
					__DeserializeLight(object, node); 
					break;
				default: CONSOLE_WARN("Invalid component found");
			}
		}
	}
}


void SceneSerializer::__DeserializeTag(GameObject& object, const YAML::Node& component)
{
	auto tag = component.as<String>();
	object.GetComponent<Tag>()->UpdateValue(tag);
}
void SceneSerializer::__DeserializeTransformation(GameObject& object, const YAML::Node& component)
{
	auto& transform = object.AddComponent<Transform>();

	auto positionNode = component["position"];
	transform.position = Vec3F{};
	transform.position.x = positionNode[0].as<f32>();
	transform.position.y = positionNode[1].as<f32>();
	transform.position.z = positionNode[2].as<f32>();

	auto scaleNode = component["scale"];
	transform.scale = Vec3F{};
	transform.scale.x = scaleNode[0].as<f32>();
	transform.scale.y = scaleNode[1].as<f32>();
	transform.scale.z = scaleNode[2].as<f32>();

	auto rotationNode = component["rotation"];
	transform.eulerAngles = Vec3F{};
	transform.eulerAngles.x = rotationNode[0].as<f32>();
	transform.eulerAngles.y = rotationNode[1].as<f32>();
	transform.eulerAngles.z = rotationNode[2].as<f32>();
}
void SceneSerializer::__DeserializeStaticMesh(GameObject& object, const YAML::Node& component)
{
	auto& instance = StaticMeshFactory::GetInstance();

	auto relative = component["path"].as<String>(); // relative path
	auto prototype = instance.GetPrototype(relative);
	if (!prototype)
	{
		auto absolute = Paths::GetModelsPath() / relative;
		prototype = instance.CreatePrototype(absolute);
	}

	auto& sm = object.AddComponent<StaticMesh>();
	prototype->Copy(sm);
}
void SceneSerializer::__DeserializeDirLight(GameObject& object, const YAML::Node& component)
{
	auto& light = object.AddComponent<DirectionalLight>();
	
	auto colorNode = component["color"];
	light.color.r = colorNode[0].as<f32>();
	light.color.g = colorNode[1].as<f32>();
	light.color.b = colorNode[2].as<f32>();

	auto dirNode = component["direction"];
	light.direction.x = dirNode[0].as<f32>();
	light.direction.y = dirNode[1].as<f32>();
	light.direction.z = dirNode[2].as<f32>();

	light.intensity = component["intensity"].as<f32>();
}
void SceneSerializer::__DeserializePointLight(GameObject& object, const YAML::Node& component)
{
	auto& light = object.AddComponent<PointLight>();

	auto colorNode = component["color"];
	light.color.r = colorNode[0].as<f32>();
	light.color.g = colorNode[1].as<f32>();
	light.color.b = colorNode[2].as<f32>();

	auto positionNode = component["position"];
	light.position.x = positionNode[0].as<f32>();
	light.position.y = positionNode[1].as<f32>();
	light.position.z = positionNode[2].as<f32>();

	light.intensity = component["intensity"].as<f32>();
	light.kl = component["kl"].as<f32>();
	light.kq = component["kq"].as<f32>();
}
void SceneSerializer::__DeserializeSpotLight(GameObject& object, const YAML::Node& component)
{
	auto& light = object.AddComponent<SpotLight>();

	auto colorNode = component["color"];
	light.color.r = colorNode[0].as<f32>();
	light.color.g = colorNode[1].as<f32>();
	light.color.b = colorNode[2].as<f32>();

	auto positionNode = component["position"];
	light.position.x = positionNode[0].as<f32>();
	light.position.y = positionNode[1].as<f32>();
	light.position.z = positionNode[2].as<f32>();

	auto directionNode = component["direction"];
	light.direction.x = directionNode[0].as<f32>();
	light.direction.y = directionNode[1].as<f32>();
	light.direction.z = directionNode[2].as<f32>();

	light.intensity = component["intensity"].as<f32>();
	light.kl = component["kl"].as<f32>();
	light.kq = component["kq"].as<f32>();
	light.thetaU = component["thetaU"].as<f32>();
	light.thetaP = component["thetaP"].as<f32>();
}
void SceneSerializer::__DeserializeLight(GameObject& object, const YAML::Node& component)
{
	auto type = component["type"].as<i32>();
	object.AddComponent<Light>(static_cast<LightType>(type));
	switch (static_cast<LightType>(type))
	{
		case LightType::Directional:
			__DeserializeDirLight(object, component);
			break;
		case LightType::Point:
			__DeserializePointLight(object, component);
			break;
		case LightType::Spot:
			__DeserializeSpotLight(object, component);
			break;
		default:
			throw std::runtime_error("invalid LightType");
	}
}

void SceneSerializer::__SerializeTag(YAML::Emitter& outEmitter, const Tag& tag)
{
	outEmitter << YAML::Key << "Tag" << YAML::Value << tag.value.data();
}
void SceneSerializer::__SerializeTransformation(YAML::Emitter& outEmitter, const Transform& transform)
{
	auto& position = transform.position;
	auto& scale = transform.scale;
	auto& degrees = transform.eulerAngles;

	outEmitter << YAML::Key << "Transformation";
	outEmitter << YAML::BeginMap;
	outEmitter << YAML::Key << "position";
	outEmitter << YAML::Flow << YAML::BeginSeq;
	outEmitter << position.x << position.y << position.z;
	outEmitter << YAML::EndSeq;
	outEmitter << YAML::Key << "scale";
	outEmitter << YAML::Flow << YAML::BeginSeq;
	outEmitter << scale.x << scale.y << scale.z;
	outEmitter << YAML::EndSeq;
	outEmitter << YAML::Key << "rotation";
	outEmitter << YAML::Flow << YAML::BeginSeq;
	outEmitter << degrees.x << degrees.y << degrees.z;
	outEmitter << YAML::EndSeq;
	outEmitter << YAML::EndMap;
}
void SceneSerializer::__SerializeStaticMesh(YAML::Emitter& outEmitter, const StaticMesh& staticMesh)
{
	auto& instance = StaticMeshFactory::GetInstance();
	auto relative = instance.GetPrototypePath(staticMesh.prototypeID);
	outEmitter << YAML::Key << "StaticMesh";
	outEmitter << YAML::BeginMap;
	outEmitter << YAML::Key << "path" << YAML::Value << relative.string();
	outEmitter << YAML::EndMap;
}
void SceneSerializer::__SerializeDirectionalLight(YAML::Emitter& outEmitter, const DirectionalLight& light)
{
	outEmitter << YAML::Key << "color";
	outEmitter << YAML::Flow << YAML::BeginSeq;
	outEmitter << light.color.x << light.color.y << light.color.z;
	outEmitter << YAML::EndSeq;
	outEmitter << YAML::Key << "direction";
	outEmitter << YAML::Flow << YAML::BeginSeq;
	outEmitter << light.direction.x << light.direction.y << light.direction.z;
	outEmitter << YAML::EndSeq;
	outEmitter << YAML::Key << "intensity" << YAML::Value << light.intensity;
}
void SceneSerializer::__SerializePointLight(YAML::Emitter& outEmitter, const PointLight& light)
{
	outEmitter << YAML::Key << "color";
	outEmitter << YAML::Flow << YAML::BeginSeq;
	outEmitter << light.color.x << light.color.y << light.color.z;
	outEmitter << YAML::EndSeq;
	outEmitter << YAML::Key << "position";
	outEmitter << YAML::Flow << YAML::BeginSeq;
	outEmitter << light.position.x << light.position.y << light.position.z;
	outEmitter << YAML::EndSeq;
	outEmitter << YAML::Key << "intensity" << YAML::Value << light.intensity;
	outEmitter << YAML::Key << "attenuation.kl" << YAML::Value << light.kl;
	outEmitter << YAML::Key << "attenuation.kq" << YAML::Value << light.kq;
}
void SceneSerializer::__SerializeSpotLight(YAML::Emitter& outEmitter, const SpotLight& light)
{
	outEmitter << YAML::Key << "color";
	outEmitter << YAML::Flow << YAML::BeginSeq;
	outEmitter << light.color.r << light.color.g << light.color.b;
	outEmitter << YAML::EndSeq;

	outEmitter << YAML::Key << "position";
	outEmitter << YAML::Flow << YAML::BeginSeq;
	outEmitter << light.position.x << light.position.y << light.position.z;
	outEmitter << YAML::EndSeq;

	outEmitter << YAML::Key << "direction";
	outEmitter << YAML::Flow << YAML::BeginSeq;
	outEmitter << light.direction.x << light.direction.y << light.direction.z;
	outEmitter << YAML::EndSeq;

	outEmitter << YAML::Key << "intensity" << YAML::Value << light.intensity;
	outEmitter << YAML::Key << "kl" << YAML::Value << light.kl;
	outEmitter << YAML::Key << "kq" << YAML::Value << light.kq;
	outEmitter << YAML::Key << "thetaU" << YAML::Value << light.thetaU;
	outEmitter << YAML::Key << "thetaP" << YAML::Value << light.thetaP;
}
void SceneSerializer::__SerializeLight(YAML::Emitter& outEmitter, const Light& light, GameObject& object)
{
	outEmitter << YAML::Key << "Light";
	outEmitter << YAML::BeginMap;
	outEmitter << YAML::Key << "type" << YAML::Value << static_cast<i32>(light.type);
	switch (light.type)
	{
		case LightType::Directional:
		{
			auto& dirLight = *object.GetComponent<DirectionalLight>();
			__SerializeDirectionalLight(outEmitter, dirLight);
			break;
		}
		case LightType::Point:
		{
			auto& pointLight = *object.GetComponent<PointLight>();
			__SerializePointLight(outEmitter, pointLight);
			break;
		}
		case LightType::Spot:
		{
			auto& spotLight = *object.GetComponent<SpotLight>();
			__SerializeSpotLight(outEmitter, spotLight);
			break;
		}

		default:
			throw std::runtime_error("invalid LightType");
	}
	outEmitter << YAML::EndMap;
}



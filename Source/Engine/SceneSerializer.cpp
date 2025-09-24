#include "SceneSerializer.hpp"

#include "Core/Logger.hpp"
#include "Engine/Utils.hpp"
#include "Engine/Scene.hpp"
#include "Engine/Importers/StaticMeshLoader.hpp"
#include "Engine/Managers/AssetsManager.hpp"

using namespace Components;

enum class ComponentType
{
	Archetype, Tag, Camera, Transformation, StaticMesh, Light, Unknown
};
static ComponentType GetComponentType(StringView name)
{
	if (name.compare("archetype") == 0)
		return ComponentType::Archetype;
	if (name.compare("tag") == 0) 
		return ComponentType::Tag;
	if (name.compare("camera") == 0)
		return ComponentType::Camera;
	else if (name.compare("transform") == 0) 
		return ComponentType::Transformation;
	else if (name.compare("staticmesh") == 0)
		return ComponentType::StaticMesh;
	else if (name.compare("light") == 0)
		return ComponentType::Light;
	
	return ComponentType::Unknown;
}

void SceneSerializer::SerializeScene(Scene& scene, const fs::path& outPath)
{
	auto outEmitter = YAML::Emitter{};
	outEmitter << YAML::BeginMap;
	for (auto e : scene.GetEntityRegistry().view<ArchetypeIdentifier>())
	{
		auto& archetypeComp = scene.GetEntityRegistry().get<ArchetypeIdentifier>(e);
		auto archetypeId = archetypeComp.archetypeId;
		auto archetype = scene.GetArchetypeRegistry().GetArchetype(archetypeId);
		auto entity = Entity{ e, &scene.GetEntityRegistry(), archetype };

		auto entityName = Array<char, 32>{};
		std::format_to(entityName.data(), "Entity{}", static_cast<u32>(e));

		outEmitter << YAML::Key << entityName.data();
		outEmitter << YAML::BeginMap;

		outEmitter << YAML::Key << "Archetype";
		outEmitter << YAML::Value << (archetype ? archetype->GetName() : "Unknown");

		auto tag = *entity.GetComponent<Tag>();
		__SerializeTag(outEmitter, tag);

		if (auto camera = entity.GetComponent<Camera>())
			__SerializeCamera(outEmitter, *camera);

		if (auto transform = entity.GetComponent<Transform>())
			__SerializeTransform(outEmitter, *transform);

		if (auto staticMesh = entity.GetComponent<StaticMesh>())
		{
			auto assetIdentifier = entity.GetComponent<AssetIdentifier>();
			__SerializeStaticMesh(outEmitter, *staticMesh, *assetIdentifier);
		}

		if (auto light = entity.GetComponent<Light>())
			__SerializeLight(outEmitter, *light, entity);

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
	for (const auto& yamlIt : yamlScene)
	{
		if (!yamlIt.second["Archetype"])
		{
			CONSOLE_ERROR("Missing Archetype field for entity: {}", yamlIt.first.as<String>());
			continue;
		}
		auto archetypeName = yamlIt.second["Archetype"].as<String>();
		auto archetypeId = scene.GetArchetypeRegistry().GetArchetypeId(archetypeName);
		if (archetypeId == INVALID_ARCHETYPE_ID)
		{
			CONSOLE_ERROR("Unknown archetype: {} for entity: {}", archetypeName, yamlIt.first.as<String>());
			continue;
		}

		auto entity = scene.CreateEntity(archetypeId);
		for (const auto& component : yamlIt.second)
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
					__DeserializeTag(entity, node);
					break;
				case ComponentType::Camera:
					__DeserializeCamera(entity, node);
					break;
				case ComponentType::Transformation:
					__DeserializeTransform(entity, node); 
					break;
				case ComponentType::StaticMesh:    
					__DeserializeStaticMesh(entity, node); 
					break;
				case ComponentType::Light:         
					__DeserializeLight(entity, node); 
					break;
				default: CONSOLE_WARN("Invalid component found");
			}
		}
	}
}

void SceneSerializer::__DeserializeTag(Entity& entity, const YAML::Node& node) const
{
	auto tag = node.as<String>();
	entity.GetComponent<Tag>()->UpdateValue(tag);
}
void SceneSerializer::__DeserializeCamera(Entity& entity, const YAML::Node& node) const
{
	auto& camera = entity.AddComponent<Camera>();
	auto& pos = node["position"];
	camera.position.x = pos[0].as<f32>();
	camera.position.y = pos[1].as<f32>();
	camera.position.z = pos[2].as<f32>();
	
	auto rot = node["rotation"];
	camera.eulerAngles.x = rot[0].as<f32>();
	camera.eulerAngles.y = rot[1].as<f32>();
	camera.eulerAngles.z = rot[2].as<f32>();

	camera.fovH = node["fovH"].as<f32>();
	camera.nearClip = node["nearClip"].as<f32>();
	camera.farClip = node["farClip"].as<f32>();
}
void SceneSerializer::__DeserializeTransform(Entity& entity, const YAML::Node& node) const
{
	auto& transform = entity.AddComponent<Transform>();

	auto positionNode = node["position"];
	transform.position = Vec3F{};
	transform.position.x = positionNode[0].as<f32>();
	transform.position.y = positionNode[1].as<f32>();
	transform.position.z = positionNode[2].as<f32>();

	auto scaleNode = node["scale"];
	transform.scale = Vec3F{};
	transform.scale.x = scaleNode[0].as<f32>();
	transform.scale.y = scaleNode[1].as<f32>();
	transform.scale.z = scaleNode[2].as<f32>();

	auto rotationNode = node["rotation"];
	transform.eulerAngles = Vec3F{};
	transform.eulerAngles.x = rotationNode[0].as<f32>();
	transform.eulerAngles.y = rotationNode[1].as<f32>();
	transform.eulerAngles.z = rotationNode[2].as<f32>();
}
void SceneSerializer::__DeserializeStaticMesh(Entity& entity, const YAML::Node& node) const
{
	auto relative = node["path"].as<String>();
	auto absolute = Utils::GetModelsPath() / relative;
	if (!fs::exists(absolute))
	{
		auto message = std::format("file does not exist: {}", absolute.string());
		throw std::runtime_error(message);
	}

	auto& instance = AssetsManager::GetInstance();
	auto assetId = instance.GetAssetId(absolute);
	if (assetId == INVALID_ASSET_ID)
		assetId = instance.RegisterAsset(absolute);
	entity.AddComponent<AssetIdentifier>(assetId);

	auto& material = entity.AddComponent<Material>();

	auto& mesh = entity.AddComponent<StaticMesh>();
	mesh.Create();
	using Vertex = VertexLayout<Position, Normal, TextureCoord, Tangent>;
	Vertex::SetupVertexArray(mesh.vertexArray);
	auto loader = StaticMeshLoader{};
	loader.LoadDataFromFile(absolute, mesh, material);
}
void SceneSerializer::__DeserializeDirLight(Entity& entity, const YAML::Node& node) const
{
	auto& light = entity.AddComponent<DirectionalLight>();

	auto colorNode = node["color"];
	light.color.r = colorNode[0].as<f32>();
	light.color.g = colorNode[1].as<f32>();
	light.color.b = colorNode[2].as<f32>();

	auto dirNode = node["direction"];
	light.direction.x = dirNode[0].as<f32>();
	light.direction.y = dirNode[1].as<f32>();
	light.direction.z = dirNode[2].as<f32>();

	light.intensity = node["intensity"].as<f32>();
}
void SceneSerializer::__DeserializePointLight(Entity& entity, const YAML::Node& node) const
{
	auto& light = entity.AddComponent<PointLight>();

	auto colorNode = node["color"];
	light.color.r = colorNode[0].as<f32>();
	light.color.g = colorNode[1].as<f32>();
	light.color.b = colorNode[2].as<f32>();

	auto positionNode = node["position"];
	light.position.x = positionNode[0].as<f32>();
	light.position.y = positionNode[1].as<f32>();
	light.position.z = positionNode[2].as<f32>();

	light.intensity = node["intensity"].as<f32>();
	light.kl = node["kl"].as<f32>();
	light.kq = node["kq"].as<f32>();
}
void SceneSerializer::__DeserializeSpotLight(Entity& entity, const YAML::Node& node) const
{
	auto& light = entity.AddComponent<SpotLight>();

	auto colorNode = node["color"];
	light.color.r = colorNode[0].as<f32>();
	light.color.g = colorNode[1].as<f32>();
	light.color.b = colorNode[2].as<f32>();

	auto positionNode = node["position"];
	light.position.x = positionNode[0].as<f32>();
	light.position.y = positionNode[1].as<f32>();
	light.position.z = positionNode[2].as<f32>();

	auto directionNode = node["direction"];
	light.direction.x = directionNode[0].as<f32>();
	light.direction.y = directionNode[1].as<f32>();
	light.direction.z = directionNode[2].as<f32>();

	light.intensity = node["intensity"].as<f32>();
	light.kl = node["kl"].as<f32>();
	light.kq = node["kq"].as<f32>();
	light.thetaU = node["thetaU"].as<f32>();
	light.thetaP = node["thetaP"].as<f32>();
}
void SceneSerializer::__DeserializeLight(Entity& entity, const YAML::Node& node) const
{
	auto type = node["type"].as<i32>();
	entity.AddComponent<Light>(static_cast<LightType>(type));
	switch (static_cast<LightType>(type))
	{
		case LightType::Directional:
			__DeserializeDirLight(entity, node);
			break;
		case LightType::Point:
			__DeserializePointLight(entity, node);
			break;
		case LightType::Spot:
			__DeserializeSpotLight(entity, node);
			break;
		default:
			throw std::runtime_error("invalid LightType");
	}
}

void SceneSerializer::__SerializeTag(YAML::Emitter& outEmitter, const Tag& tag) const
{
	outEmitter << YAML::Key << "Tag" << YAML::Value << tag.value.data();
}
void SceneSerializer::__SerializeCamera(YAML::Emitter& outEmitter, const Components::Camera& camera) const
{
	outEmitter << YAML::Key << "Camera";
	outEmitter << YAML::BeginMap;

	outEmitter << YAML::Key << "position" << YAML::Value << YAML::Flow << YAML::BeginSeq;
	outEmitter << camera.position.x << camera.position.y << camera.position.z;
	outEmitter << YAML::EndSeq;

	outEmitter << YAML::Key << "rotation" << YAML::Value << YAML::Flow << YAML::BeginSeq;
	outEmitter << camera.eulerAngles.x << camera.eulerAngles.y << camera.eulerAngles.z;
	outEmitter << YAML::EndSeq;

	outEmitter << YAML::Key << "fovH" << YAML::Value << camera.fovH;
	outEmitter << YAML::Key << "nearClip" << YAML::Value << camera.nearClip;
	outEmitter << YAML::Key << "farClip" << YAML::Value << camera.farClip;

	outEmitter << YAML::EndMap;
}
void SceneSerializer::__SerializeTransform(YAML::Emitter& outEmitter, const Transform& transform) const
{
	auto& position = transform.position;
	auto& scale = transform.scale;
	auto& degrees = transform.eulerAngles;

	outEmitter << YAML::Key << "Transform";
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
void SceneSerializer::__SerializeStaticMesh(YAML::Emitter& outEmitter, 
																						const StaticMesh& staticMesh,
																						AssetIdentifier assetIdentifier) const
{
	auto& instance = AssetsManager::GetInstance();
	auto absolute = instance.GetAssetPath(assetIdentifier.assetId);
	if (!absolute)
		throw std::runtime_error("static mesh does not have a valid model path");
	
	auto relative = fs::relative(*absolute.value(), Utils::GetModelsPath());
	outEmitter << YAML::Key << "StaticMesh";
	outEmitter << YAML::BeginMap;
	outEmitter << YAML::Key << "path" << YAML::Value << relative.string();
	outEmitter << YAML::EndMap;
}
void SceneSerializer::__SerializeDirectionalLight(YAML::Emitter& outEmitter, const DirectionalLight& light) const
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
void SceneSerializer::__SerializePointLight(YAML::Emitter& outEmitter, const PointLight& light) const
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
	outEmitter << YAML::Key << "kl" << YAML::Value << light.kl;
	outEmitter << YAML::Key << "kq" << YAML::Value << light.kq;
}
void SceneSerializer::__SerializeSpotLight(YAML::Emitter& outEmitter, const SpotLight& light) const
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
void SceneSerializer::__SerializeLight(YAML::Emitter& outEmitter, const Light& light, Entity& entity) const
{
	outEmitter << YAML::Key << "Light";
	outEmitter << YAML::BeginMap;
	outEmitter << YAML::Key << "type" << YAML::Value << static_cast<i32>(light.type);
	switch (light.type)
	{
		case LightType::Directional:
		{
			auto& dirLight = *entity.GetComponent<DirectionalLight>();
			__SerializeDirectionalLight(outEmitter, dirLight);
			break;
		}
		case LightType::Point:
		{
			auto& pointLight = *entity.GetComponent<PointLight>();
			__SerializePointLight(outEmitter, pointLight);
			break;
		}
		case LightType::Spot:
		{
			auto& spotLight = *entity.GetComponent<SpotLight>();
			__SerializeSpotLight(outEmitter, spotLight);
			break;
		}

		default:
			throw std::runtime_error("invalid LightType");
	}
	outEmitter << YAML::EndMap;
}



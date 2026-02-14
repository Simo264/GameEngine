#include "SceneSerializer.hpp"

#include "Utils/YAMLParser.hpp"
#include "Utils/Logger.hpp"
#include "Engine/Paths.hpp"
#include "Engine/ECS/Scene.hpp"
#include "Engine/ECS/Builders/StaticMeshEntityBuilder.hpp"
#include "Engine/ECS/Builders/CameraEntityBuilder.hpp"
#include "Engine/ECS/Builders/LightSourceEntityBuilder.hpp"
#include "Engine/Managers/AssetsManager.hpp"

using namespace Components;

void SceneSerializer::Save(const fs::path& absolutePath)
{
	auto outEmitter = YAML::Emitter{};
	outEmitter << YAML::BeginMap;
	for (auto e : _scene->GetEntityRegistry().view<ArchetypeIdentifier>())
	{
		auto& archetypeComp = _scene->GetEntityRegistry().get<ArchetypeIdentifier>(e);
		auto archetypeId = archetypeComp.archetypeId;
		auto& archetype = _scene->GetArchetypeRegistry().GetArchetype(archetypeId);
		auto archetypeName = StringView(archetype.GetName());

		auto& tagComponent = _scene->GetEntityComponent<Tag>(e);

		auto entity = Entity{};
		auto entityName = Array<char, 32>{};
		std::format_to(entityName.data(), "Entity{}", static_cast<u32>(e));

		outEmitter << YAML::Key << entityName.data();
		outEmitter << YAML::BeginMap;
		outEmitter << YAML::Key << "Archetype";
		outEmitter << YAML::Value << archetypeName;
		outEmitter << YAML::Key << "Tag";
		outEmitter << YAML::Value << tagComponent.value.data();

		if(archetypeName.compare("StaticMesh"))
			__SerializeArchetypeStaticMesh(outEmitter, entity);
		
		else if (archetypeName.compare("LightSource"))
			__SerializeArchetypeLightSource(outEmitter, entity);
		
		else if (archetypeName.compare("Camera"))
			__SerializeArchetypeCamera(outEmitter, entity);
		
		else
			throw std::runtime_error("INVALID_ARCHETYPE_ID");

		outEmitter << YAML::EndMap;
	}

	outEmitter << YAML::EndMap;
	auto outFile = OutputFileStream(absolutePath);
	outFile << outEmitter.c_str();
	outFile.close();
}
void SceneSerializer::Load(const fs::path& absolutePath)
{
	if (!fs::exists(absolutePath))
		throw std::runtime_error(std::format("{} does not exist!", absolutePath.string()));

	auto yamlScene = YAML::LoadFile(absolutePath.string());
	for (const auto& yamlEntityNode : yamlScene)
	{
		auto& archetypeNode = yamlEntityNode.second["Archetype"];
		auto archetypeName = archetypeNode.as<String>();
		auto archetypeId = _scene->GetArchetypeRegistry().GetArchetypeId(archetypeName);
		if(archetypeId == INVALID_ARCHETYPE_ID)
			throw std::runtime_error("INVALID_ARCHETYPE_ID");

		auto& tagNode = yamlEntityNode.second["Tag"];
		auto tagName = tagNode.as<String>();
		if (archetypeName.compare("StaticMesh") == 0)
			__DeserializeArchetypeStaticMesh(yamlEntityNode, archetypeId, tagName);
		else if (archetypeName.compare("LightSource") == 0)
			__DeserializeArchetypeLightSource(yamlEntityNode, archetypeId, tagName);
		else if (archetypeName.compare("Camera") == 0)
			__DeserializeArchetypeCamera(yamlEntityNode, archetypeId, tagName);
		else
			throw std::runtime_error("Unkown archetype");
	}
}

void SceneSerializer::__DeserializeArchetypeStaticMesh(const YAML::detail::iterator_value& yamlEntityNode,
																											 ArchetypeId archetypeId,
																											 StringView tag)
{
	auto& transformNode = yamlEntityNode.second["Transform"];
	auto transform = Transform{};
	transform.position = transformNode["position"].as<Vec3F>();
	transform.scale = transformNode["scale"].as<Vec3F>();
	transform.eulerAngles = transformNode["rotation"].as<Vec3F>();

	auto& meshNode = yamlEntityNode.second["StaticMesh"];
	auto relativeMeshPath = meshNode["path"].as<String>();
	auto absoluteMeshPath = GetModelsPath() / relativeMeshPath;
	if (!fs::exists(absoluteMeshPath))
		throw std::runtime_error(std::format("file does not exist: {}", absoluteMeshPath.string()));

	auto builder = StaticMeshEntityBuilder{ *_scene };
	builder.WithArchetypeIdentifier(archetypeId);
	builder.WithTag(tag);
	builder.WithTransform(transform);
	builder.WithModelPath(absoluteMeshPath);
	builder.Build();
}
void SceneSerializer::__DeserializeArchetypeLightSource(const YAML::detail::iterator_value& yamlEntityNode,
																												ArchetypeId archetypeId,
																												StringView tag)
{
	auto& lightNode = yamlEntityNode.second["Light"];
	auto lightType = static_cast<LightType>(lightNode["type"].as<u32>());
	switch (lightType)
	{
		case LightType::Directional:
		{
			auto color = lightNode["color"].as<Vec3F>();
			auto intensity = lightNode["intensity"].as<f32>();
			auto direction = lightNode["direction"].as<Vec3F>();

			auto light = DirectionalLight{};
			light.color = Vec4F(color, 0.f);
			light.intensity = intensity;
			light.direction = Vec4F(direction, 0.f);

			auto builder = LightSourceEntityBuilder{ *_scene };
			builder.WithArchetypeIdentifier(archetypeId);
			builder.WithTag(tag);
			builder.WithLightType(lightType);
			auto entity = builder.Build();
			
			auto& lightComponent = _scene->GetEntityComponent<DirectionalLight>(entity);
			lightComponent = light;
			break;
		}
		case LightType::Point:
		{
			auto color = lightNode["color"].as<Vec3F>();
			auto position = lightNode["position"].as<Vec3F>();

			auto light = PointLight{};
			light.color = Vec4F(color, 0.f);
			light.intensity = lightNode["intensity"].as<f32>();
			light.position = Vec4F(position, 0.f);
			light.kl = lightNode["kl"].as<f32>();
			light.kq = lightNode["kq"].as<f32>();

			auto builder = LightSourceEntityBuilder{ *_scene };
			builder.WithArchetypeIdentifier(archetypeId);
			builder.WithTag(tag);
			builder.WithLightType(lightType);
			auto entity = builder.Build();
			
			auto& lightComponent = _scene->GetEntityComponent<PointLight>(entity);
			lightComponent = light;
			break;
		}
		case LightType::Spot:
		{
			auto color = lightNode["color"].as<Vec3F>();
			auto position = lightNode["position"].as<Vec3F>();
			auto direction = lightNode["direction"].as<Vec3F>();

			auto light = SpotLight{};
			light.color = Vec4F(color, 0.f);
			light.intensity = lightNode["intensity"].as<f32>();
			light.position = Vec4F(position, 0.f);
			light.direction = Vec4F(direction, 0.f);
			light.kl = lightNode["kl"].as<f32>();
			light.kq = lightNode["kq"].as<f32>();
			light.thetaU = lightNode["thetaU"].as<f32>();
			light.thetaP = lightNode["thetaP"].as<f32>();

			auto builder = LightSourceEntityBuilder{ *_scene };
			builder.WithArchetypeIdentifier(archetypeId);
			builder.WithTag(tag);
			builder.WithLightType(lightType);
			auto entity = builder.Build();

			auto& lightComponent = _scene->GetEntityComponent<SpotLight>(entity);
			lightComponent = light;
			break;
		}
		default:
			throw std::runtime_error("Unkown light type");
	}
}
void SceneSerializer::__DeserializeArchetypeCamera(const YAML::detail::iterator_value& yamlEntityNode,
																									 ArchetypeId archetypeId,
																									 StringView tag)
{
	auto& cameraNode = yamlEntityNode.second["Camera"];

	auto camera = Camera{};
	camera.position = cameraNode["position"].as<Vec3F>();
	camera.eulerAngles = cameraNode["rotation"].as<Vec3F>();
	camera.fovH = cameraNode["fovH"].as<f32>();
	camera.nearClip = cameraNode["nearClip"].as<f32>();
	camera.farClip = cameraNode["farClip"].as<f32>();

	auto builder = CameraEntityBuilder{ *_scene };
	builder.WithArchetypeIdentifier(archetypeId);
	builder.WithTag(tag);
	auto entity = builder.Build();
	auto& cameraComponent = _scene->GetEntityComponent<Camera>(entity);
	cameraComponent = camera;
}

void SceneSerializer::__SerializeArchetypeStaticMesh(YAML::Emitter& outEmitter, Entity entity)
{
	auto& transform = _scene->GetEntityComponent<Transform>(entity);
	outEmitter << YAML::Key << "Transform";
	outEmitter << YAML::BeginMap;
	outEmitter << YAML::Key << "position" << YAML::Value
		<< YAML::Flow
		<< YAML::BeginSeq
		<< transform.position.x << transform.position.y << transform.position.z
		<< YAML::EndSeq;
	outEmitter << YAML::Key << "scale" << YAML::Value
		<< YAML::Flow
		<< YAML::BeginSeq
		<< transform.scale.x << transform.scale.y << transform.scale.z
		<< YAML::EndSeq;
	outEmitter << YAML::Key << "rotation" << YAML::Value
		<< YAML::Flow
		<< YAML::BeginSeq
		<< transform.eulerAngles.x << transform.eulerAngles.y << transform.eulerAngles.z
		<< YAML::EndSeq;
	outEmitter << YAML::EndMap;

	auto& assetsManager = AssetsManager::GetInstance();
	auto& assetIdComp = _scene->GetEntityComponent<AssetIdentifier>(entity);
	auto modelPath = assetsManager.GetAssetPath(assetIdComp.assetId);
	outEmitter << YAML::Key << "StaticMesh";
	outEmitter << YAML::BeginMap;
	outEmitter << YAML::Key << "path" << YAML::Value << modelPath.value()->string();
	outEmitter << YAML::EndMap;
}
void SceneSerializer::__SerializeArchetypeLightSource(YAML::Emitter& outEmitter, Entity entity)
{
	auto& lightComponent = _scene->GetEntityComponent<Light>(entity);
	auto type = lightComponent.type;

	outEmitter << YAML::Key << "Light";
	outEmitter << YAML::BeginMap;
	outEmitter << YAML::Key << "type" << YAML::Value << static_cast<i32>(type);
	switch (type)
	{
		case LightType::Directional:
		{
			auto& light = _scene->GetEntityComponent<DirectionalLight>(entity);
			outEmitter << YAML::Key << "color";
			outEmitter << YAML::Flow << YAML::BeginSeq;
			outEmitter << light.color.r << light.color.g << light.color.b;
			outEmitter << YAML::EndSeq;
			outEmitter << YAML::Key << "direction";
			outEmitter << YAML::Flow << YAML::BeginSeq;
			outEmitter << light.direction.x << light.direction.y << light.direction.z;
			outEmitter << YAML::EndSeq;
			outEmitter << YAML::Key << "intensity" << YAML::Value << light.intensity;
			break;
		}
		case LightType::Point:
		{
			auto& light = _scene->GetEntityComponent<PointLight>(entity);
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
			break;
		}
		case LightType::Spot:
		{
			auto& light = _scene->GetEntityComponent<SpotLight>(entity);
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
			break;
		}
		default:
			throw std::runtime_error("Invalid light type");
	}
	outEmitter << YAML::EndMap;
}
void SceneSerializer::__SerializeArchetypeCamera(YAML::Emitter& outEmitter, Entity entity)
{
	auto& camera =  _scene->GetEntityComponent<Camera>(entity);
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

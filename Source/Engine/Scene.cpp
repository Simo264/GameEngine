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

static void DeserializeTag(GameObject &object, const YAML::Node &component)
{
	String tag = component.as<String>();
	object.GetComponent<Tag>()->UpdateValue(tag);
}
static void DeserializeTransform(GameObject &object, const YAML::Node &component)
{
	Transform &transform = object.AddComponent<Transform>();
	YAML::Node node = component["position"];
	transform.position = {
			node[0].as<f32>(),
			node[1].as<f32>(),
			node[2].as<f32>()};
	node = component["scale"];
	transform.scale = {
			node[0].as<f32>(),
			node[1].as<f32>(),
			node[2].as<f32>()};
	node = component["rotation"];
	transform.rotation = {
			node[0].as<f32>(),
			node[1].as<f32>(),
			node[2].as<f32>()};
	transform.UpdateTransformation();
}
static void DeserializeStaticMesh(GameObject &object, const YAML::Node &component)
{
	ModelsManager &modelsManager = ModelsManager::Get();

	fs::path path = component["path"].as<String>();
	const StaticMesh *staticMesh = modelsManager.FindStaticMesh(path);
	if (!staticMesh)
		staticMesh = &modelsManager.CreateStaticMesh(path);

	StaticMesh &staticMeshComponent = object.AddComponent<StaticMesh>();
	staticMesh->Clone(staticMeshComponent);
}
static void DeserializeSkeletalMesh(GameObject &object, const YAML::Node &component)
{
	ModelsManager &modelsManager = ModelsManager::Get();
	AnimationsManager &animationsManager = AnimationsManager::Get();

	SkeletalMesh &skeletalMeshComponent = object.AddComponent<SkeletalMesh>();
	Animator &animatorComponent = object.AddComponent<Animator>();

	// The relative path to "Assets/Models/Skeletal" (e.g. "Mutant/Mutant.gltf")
	fs::path path = component["path"].as<String>();
	const SkeletalMesh *skeleton = modelsManager.FindSkeletalMesh(path);
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
			for (auto &p : relativeAnims)
				p = parent / p;
		}
		animationsManager.LoadAnimations(*skeleton, relativeAnims);
	}

	skeleton->Clone(skeletalMeshComponent);
	animatorComponent.SetTargetSkeleton(skeletalMeshComponent);
}
static void DeserializeDirLight(GameObject &object, const YAML::Node &component)
{
	auto &light = object.AddComponent<DirectionalLight>();
	YAML::Node node = component["color"];
	light.color = {
			node[0].as<f32>(),
			node[1].as<f32>(),
			node[2].as<f32>()};
	node = component["direction"];
	light.direction = {
			node[0].as<f32>(),
			node[1].as<f32>(),
			node[2].as<f32>()};
	light.intensity = component["intensity"].as<f32>();
}
static void DeserializePointLight(GameObject &object, const YAML::Node &component)
{
	auto &light = object.AddComponent<PointLight>();
	YAML::Node node = component["color"];
	light.color = {
			node[0].as<f32>(),
			node[1].as<f32>(),
			node[2].as<f32>()};
	node = component["position"];
	light.position = {
			node[0].as<f32>(),
			node[1].as<f32>(),
			node[2].as<f32>()};
	light.intensity = component["intensity"].as<f32>();
	light.attenuation.range = component["attenuation.range"].as<i32>();
	light.attenuation.kl = component["attenuation.kl"].as<f32>();
	light.attenuation.kq = component["attenuation.kq"].as<f32>();
}
static void DeserializeSpotLight(GameObject &object, const YAML::Node &component)
{
	auto &light = object.AddComponent<SpotLight>();
	YAML::Node node = component["color"];
	light.color = {
			node[0].as<f32>(),
			node[1].as<f32>(),
			node[2].as<f32>()};
	node = component["direction"];
	light.direction = {
			node[0].as<f32>(),
			node[1].as<f32>(),
			node[2].as<f32>()};
	node = component["position"];
	light.position = {
			node[0].as<f32>(),
			node[1].as<f32>(),
			node[2].as<f32>()};
	light.intensity = component["intensity"].as<f32>();
	light.attenuation.range = component["attenuation.range"].as<i32>();
	light.attenuation.kl = component["attenuation.kl"].as<f32>();
	light.attenuation.kq = component["attenuation.kq"].as<f32>();
	light.cutOff = component["cutOff"].as<f32>();
	light.outerCutOff = component["outerCutOff"].as<f32>();
}
static void DeserializeLight(GameObject &object, const YAML::Node &component)
{
	i32 type = component["type"].as<i32>();
	object.AddComponent<Light>(static_cast<LightType>(type));
	switch (static_cast<LightType>(type))
	{
	case LightType::DIRECTIONAL:
		DeserializeDirLight(object, component);
		break;
	case LightType::POINT:
		DeserializePointLight(object, component);
		break;
	case LightType::SPOT:
		DeserializeSpotLight(object, component);
		break;
	default:
		throw std::runtime_error("invalid LightType");
	}
}
static UnorderedMap<String, std::function<void(GameObject &, const YAML::Node &)>> deserializationMap =
		{
				{"Tag", DeserializeTag},
				{"Transform", DeserializeTransform},
				{"StaticMesh", DeserializeStaticMesh},
				{"SkeletalMesh", DeserializeSkeletalMesh},
				{"Light", DeserializeLight},
};

static void SerializeTag(YAML::Emitter &outEmitter, const Tag &tag)
{
	outEmitter << YAML::Key << "Tag" << YAML::Value << tag.value.data();
}
static void SerializeTransform(YAML::Emitter &outEmitter, const Transform &transform)
{
	outEmitter << YAML::Key << "Transform";
	outEmitter << YAML::BeginMap;
	outEmitter << YAML::Key << "position";
	outEmitter << YAML::Flow << YAML::BeginSeq;
	outEmitter << transform.position.x << transform.position.y << transform.position.z;
	outEmitter << YAML::EndSeq;
	outEmitter << YAML::Key << "scale";
	outEmitter << YAML::Flow << YAML::BeginSeq;
	outEmitter << transform.scale.x << transform.scale.y << transform.scale.z;
	outEmitter << YAML::EndSeq;
	outEmitter << YAML::Key << "rotation";
	outEmitter << YAML::Flow << YAML::BeginSeq;
	outEmitter << transform.rotation.x << transform.rotation.y << transform.rotation.z;
	outEmitter << YAML::EndSeq;
	outEmitter << YAML::EndMap;
}
static void SerializeStaticMesh(YAML::Emitter &outEmitter, const StaticMesh &staticMesh)
{
	ModelsManager &modelsManager = ModelsManager::Get();
	const fs::path &path = *modelsManager.GetStaticMeshPath(staticMesh.id);
	outEmitter << YAML::Key << "StaticMesh";
	outEmitter << YAML::BeginMap;
	outEmitter << YAML::Key << "path" << YAML::Value << path.string();
	outEmitter << YAML::EndMap;
}
static void SerializeSkeletalMesh(YAML::Emitter &outEmitter, const SkeletalMesh &skeletalMesh)
{
	ModelsManager &modelsManager = ModelsManager::Get();
	const fs::path &path = *modelsManager.GetSkeletalMeshPath(skeletalMesh.id);
	outEmitter << YAML::Key << "SkeletalMesh";
	outEmitter << YAML::BeginMap;
	outEmitter << YAML::Key << "path" << YAML::Value << path.string();
	outEmitter << YAML::EndMap;
}
static void SerializeDirectionalLight(YAML::Emitter &outEmitter, const DirectionalLight &light)
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
static void SerializePointLight(YAML::Emitter &outEmitter, const PointLight &light)
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
	outEmitter << YAML::Key << "attenuation.range" << YAML::Value << light.attenuation.range;
	outEmitter << YAML::Key << "attenuation.kl" << YAML::Value << light.attenuation.kl;
	outEmitter << YAML::Key << "attenuation.kq" << YAML::Value << light.attenuation.kq;
}
static void SerializeSpotLight(YAML::Emitter &outEmitter, const SpotLight &light)
{
	outEmitter << YAML::Key << "color";
	outEmitter << YAML::Flow << YAML::BeginSeq;
	outEmitter << light.color.x << light.color.y << light.color.z;
	outEmitter << YAML::EndSeq;
	outEmitter << YAML::Key << "direction";
	outEmitter << YAML::Flow << YAML::BeginSeq;
	outEmitter << light.direction.x << light.direction.y << light.direction.z;
	outEmitter << YAML::EndSeq;
	outEmitter << YAML::Key << "position";
	outEmitter << YAML::Flow << YAML::BeginSeq;
	outEmitter << light.position.x << light.position.y << light.position.z;
	outEmitter << YAML::EndSeq;
	outEmitter << YAML::Key << "intensity" << YAML::Value << light.intensity;
	outEmitter << YAML::Key << "attenuation.range" << YAML::Value << light.attenuation.range;
	outEmitter << YAML::Key << "attenuation.kl" << YAML::Value << light.attenuation.kl;
	outEmitter << YAML::Key << "attenuation.kq" << YAML::Value << light.attenuation.kq;
	outEmitter << YAML::Key << "cutOff" << YAML::Value << light.cutOff;
	outEmitter << YAML::Key << "outerCutOff" << YAML::Value << light.outerCutOff;
}
static void SerializeLight(YAML::Emitter &outEmitter, const Light &light, GameObject &object)
{
	outEmitter << YAML::Key << "Light";
	outEmitter << YAML::BeginMap;
	outEmitter << YAML::Key << "type" << YAML::Value << static_cast<i32>(light.type);
	switch (light.type)
	{
	case LightType::DIRECTIONAL:
	{
		auto &dirLight = *object.GetComponent<DirectionalLight>();
		SerializeDirectionalLight(outEmitter, dirLight);
		break;
	}
	case LightType::POINT:
	{
		auto &pointLight = *object.GetComponent<PointLight>();
		SerializePointLight(outEmitter, pointLight);
		break;
	}
	case LightType::SPOT:
	{
		const auto &spotLight = *object.GetComponent<SpotLight>();
		SerializeSpotLight(outEmitter, spotLight);
		break;
	}
	default:
		throw std::runtime_error("invalid LightType");
	}
	outEmitter << YAML::EndMap;
}

// -----------------------------------
//								PUBLIC
// -----------------------------------

Scene::Scene(const fs::path &loadFrom)
{
	LoadFromFile(loadFrom);
}
GameObject Scene::CreateObject(StringView objName)
{
	entt::entity id = _registry.create();

	Array<char, 32> defaultTag{};
	if (objName.empty())
		std::format_to_n(defaultTag.begin(), defaultTag.size(), "Object_{}", static_cast<u32>(id));
	else
		std::copy(objName.begin(), objName.end(), defaultTag.begin());

	GameObject object{id, &_registry};
	object.AddComponent<Tag>(defaultTag.data());
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
void Scene::LoadFromFile(const fs::path &loadFrom)
{
	CONSOLE_INFO("Loading scene {}...", loadFrom.relative_path().string());
	DeserializeScene(loadFrom);
}
void Scene::SaveToFile(const fs::path &out)
{
	CONSOLE_INFO("Saving scene {}...", out.relative_path().string());
	SerializeScene(out);
}

// -----------------------------------
//								PRIVATE
// -----------------------------------

void Scene::SerializeScene(const fs::path &out)
{
	YAML::Emitter outEmitter;
	outEmitter << YAML::BeginMap;
	for (auto entity : Reg().view<entt::entity>())
	{
		GameObject object{entity, &Reg()};

		Array<char, 32> entityName{};
		std::format_to_n(entityName.data(), entityName.size(), "Entity{}", static_cast<u32>(object.id));

		outEmitter << YAML::Key << entityName.data();
		outEmitter << YAML::BeginMap;

		Tag &tag = *object.GetComponent<Tag>();
		SerializeTag(outEmitter, tag);

		if (Transform *transform = object.GetComponent<Transform>())
			SerializeTransform(outEmitter, *transform);

		if (StaticMesh *staticMesh = object.GetComponent<StaticMesh>())
			SerializeStaticMesh(outEmitter, *staticMesh);

		if (SkeletalMesh *skeleton = object.GetComponent<SkeletalMesh>())
			SerializeSkeletalMesh(outEmitter, *skeleton);

		if (Light *light = object.GetComponent<Light>())
			SerializeLight(outEmitter, *light, object);

		outEmitter << YAML::EndMap;
	}

	outEmitter << YAML::EndMap;
	OutputFileStream outFile(out);
	outFile << outEmitter.c_str();
	outFile.close();
}
void Scene::DeserializeScene(const fs::path &loadFrom)
{
	if (!fs::exists(loadFrom))
		throw std::runtime_error(std::format("{} does not exist!", loadFrom.string()));

	YAML::Node scene = YAML::LoadFile(loadFrom.string());
	for (const auto &entity : scene)
	{
		GameObject object = CreateObject();

		for (const auto &component : entity.second)
		{
			String componentName = component.first.as<String>();
			auto it = deserializationMap.find(componentName);
			if (it == deserializationMap.end())
			{
				CONSOLE_WARN("Invalid component found");
				continue;
			}

			// Call the appropriate deserialisation function
			it->second(object, component.second);
		}
	}
}

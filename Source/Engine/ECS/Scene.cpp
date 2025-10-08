#include "Scene.hpp"

void Scene::InitializeDefaultArchetypes()
{
	// Camera
	auto cameraArchetype = Archetype{ "Camera" };
	cameraArchetype.AllowComponents<Components::Camera>();
	RegisterArchetype(cameraArchetype);

	auto lightSourceArchetype = Archetype{ "LightSource" };
	lightSourceArchetype.AllowComponents<
		Components::Light,
		Components::DirectionalLight,
		Components::PointLight,
		Components::SpotLight>();
	RegisterArchetype(lightSourceArchetype);

	auto staticMeshArchetype = Archetype{ "StaticMesh" };
	staticMeshArchetype.AllowComponents<
		Components::AssetIdentifier,
		Components::Transform,
		Components::StaticMesh,
		Components::Material>();
	RegisterArchetype(staticMeshArchetype);
}

Entity Scene::CreateEntity(ArchetypeId archetypeId, StringView tag)
{
	auto entity = Entity{ _entityRegistry.create() };
	_entityRegistry.emplace<Components::ArchetypeIdentifier>(entity.Id(), archetypeId);
	_entityRegistry.emplace<Components::Tag>(entity.Id(), tag.data());
	return entity;
}
void Scene::DestroyEntity(EntityId id)
{
	if (_entityRegistry.valid(id))
		_entityRegistry.destroy(id);
}
void Scene::Clear()
{
	auto bufferIds = Vector<BufferId>{};
	auto vertexArrayIds = Vector<VertexArrayId>{};
	auto view = _entityRegistry.view<StaticMesh>();
	for (auto entity : view)
	{
		auto& mesh = view.get<StaticMesh>(entity);
		if (mesh.vertexArray.Valid())
		{
			vertexArrayIds.push_back(mesh.vertexArray.id);
			mesh.vertexArray.id = INVALID_VERTEXARRAY_ID;
		}
		if (mesh.vertexBuffer.Valid())
		{
			bufferIds.push_back(mesh.vertexBuffer.id);
			mesh.vertexBuffer.id = INVALID_BUFFER_ID;
		}
		if (mesh.indexBuffer.Valid())
		{
			bufferIds.push_back(mesh.indexBuffer.id);
			mesh.indexBuffer.id = INVALID_BUFFER_ID;
		}
	}

	// Batch delete
	if (!vertexArrayIds.empty())
		VertexArray::DeleteArrays(vertexArrayIds.size(), vertexArrayIds.data());
	if (!bufferIds.empty())
		Buffer::DeleteBuffers(bufferIds.size(), bufferIds.data());

	_entityRegistry.clear();
}
void Scene::RegisterArchetype(const Archetype& archetype)
{
	_archetypeRegistry.RegisterArchetype(archetype);
}

Optional<Entity> Scene::FindEntityWithTag(StringView tagName)
{
	auto view = _entityRegistry.view<Components::Tag>();
	for (auto entity : view)
	{
		auto& tag = GetEntityComponent<Components::Tag>(entity);
		if (tagName.compare(tag.value.data()) == 0)
			return Entity{ entity };
	}
	return std::nullopt;
}



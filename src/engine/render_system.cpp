#include "RenderSystem.hpp"
#include "Utils/Logger.hpp"

#include "Engine/ECS/Scene.hpp"
#include "Engine/Graphics/RenderAPI.hpp"
#include "Engine/Graphics/Shader.hpp"
#include "Engine/Managers/TexturesManager.hpp"

using namespace Components;

void RenderSystem::Render(Scene& scene)
{
	if (!_program)
	{
		CONSOLE_WARN("Invalid program");
		return;
	}

	_program->Use();

	// entities with material component
	auto viewWithMaterial = scene.GetEntityRegistry().view<StaticMesh, Transform, Material>();
	for (auto [entity, mesh, transform, material] : viewWithMaterial.each())
	{
		__BindMaterial(material);
		_program->SetUniformMat4f("u_Transform", transform.GetTransformation());
		RenderAPI::Draw(RenderMode::TRIANGLES, mesh);
	}

	__UnbindMaterial();

	// entities without material
	auto viewWithoutMaterial = scene.GetEntityRegistry().view<StaticMesh, Transform>(entt::exclude<Material>);
	for (auto [entity, mesh, transform] : viewWithoutMaterial.each())
	{
		_program->SetUniformMat4f("u_Transform", transform.GetTransformation());
		RenderAPI::Draw(RenderMode::TRIANGLES, mesh);
	}
}


void RenderSystem::__BindMaterial(const Components::Material& material)
{
	material.albedo.BindTextureUnit(10);
	material.normalMap.BindTextureUnit(11);
}
void RenderSystem::__UnbindMaterial()
{
	const auto& fallback = TexturesManager::GetInstance().GetFallbackTexture();
	fallback.BindTextureUnit(10);
	fallback.BindTextureUnit(11);
}

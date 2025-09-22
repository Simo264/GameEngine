#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Base.hpp"
#include "Engine/Components/Components.hpp"
#include "Engine/Graphics/Texture2D.hpp"

class Entity;
class EntityArchetype;

class InspectorPanel
{
public:
	InspectorPanel();

	void Render(StringView windowName, Entity entity);

	bool isOpen;

private:
	void __ListAllComponents(Entity entity);
	
	void __RenderArchetype(u32 archetypeId, StringView archetypeName) const;
	void __RenderTagComponent(Components::Tag& tag) const;

	void __RenderTransformComponent(Components::Transform& transform) const;
	void __RenderTransformTableRow(StringView label, Vec3F& values, Vec3F resetValues) const;

	void __RenderCameraComponent(Components::Camera& camera) const;
	void __RenderProjectionRow(StringView label, Components::Camera& camera, f32 resetValue) const;

	//bool __ButtonCentered(StringView label, Vec2I size);
	//void __NewComponentPopup(Entity entity);
	//void __AddTransformComponent(Entity entity);
	//void __AddStaticMeshComponent(Entity entity);
	//void __AddLightComponent(Entity entity);

	Texture2D _resetIcon;
};

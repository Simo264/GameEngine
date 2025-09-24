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
	
	void __RenderTagComponent(Components::Tag& tag) const;
	void __RenderTransformComponent(Components::Transform& transform) const;
	void __RenderCameraComponent(Components::Camera& camera) const;
	void __RenderStaticMeshComponent(Components::StaticMesh& mesh) const;
	void __RenderMaterialComponent(Components::Material& material) const;
	
	void __RenderDirectionalLight(Components::DirectionalLight& light) const;
	void __RenderPointLight(Components::PointLight& light) const;
	void __RenderSpotLight(Components::SpotLight& light) const;
	
	void __AttenuationPresetCombo(StringView id, f32& kl, f32& kq) const;

	// Helper function for a 3-float slider
	void __Drag3F(StringView label, 
							  StringView id,
							  Vec3F& values, 
							  f32 speed, 
							  f32 minVal, 
							  f32 maxVal, 
							  Array<StringView, 3> format) const;
	// Helper function for a single float slider
	void __Drag1F(StringView label, 
								StringView id,
								f32& value, 
								f32 speed, 
								f32 minVal, 
								f32 maxVal,
								StringView format,
								i32 flags = 0) const;
	// Helper function for a single float slider
	void __Slider1F(StringView label,
									StringView id,
									f32& value,
									f32 minVal,
									f32 maxVal,
									StringView format,
									i32 flags = 0) const;
	// Helper function for a Color Edit
	void __ColorEdit3(StringView label, 
										StringView id, 
										Vec4F& color) const;

	

	Texture2D _resetIcon;
};

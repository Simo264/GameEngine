#pragma once

#include "Core/Math/Math.hpp"


/* --------------------------------------------
			TransformComponent Systems
	-------------------------------------------- */
void UpdatePosition(class TransformComponent& component, const Vec3f& position);
void UpdateScale(class TransformComponent& component, const Vec3f& scale);
void UpdateRotation(class TransformComponent& component, const Vec3f& rotation);


/* --------------------------------------------
			Render Systems
	-------------------------------------------- */
void RenderMesh(struct StaticMeshComponent& component, const Mat4f&, class Shader&);
void RenderLight(struct DirLightComponent& component, class Shader&);
void RenderLight(struct PointLightComponent& component, class Shader&);
void RenderLight(struct SpotLightComponent& component, class Shader&);



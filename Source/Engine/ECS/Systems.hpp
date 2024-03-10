#pragma once

#include "Core/Math/Math.hpp"

/* --------------------------------------------
			TransformComponent Systems
	-------------------------------------------- */
void UpdatePosition(struct TransformComponent& transform, const Vec3f& position);
void UpdateScale(struct TransformComponent& transform, const Vec3f& scale);
void UpdateRotation(struct TransformComponent& transform, const Vec3f& rotation);

/* --------------------------------------------
			CameraComponent Systems
	-------------------------------------------- */


/* --------------------------------------------
			Render Systems
	-------------------------------------------- */
void RenderMesh(struct StaticMeshComponent& mesh, const Mat4f& transform, class Shader& shader);
void RenderLight(struct DirLightComponent& light, class Shader& shader);
void RenderLight(struct PointLightComponent& light, class Shader& shader);
void RenderLight(struct SpotLightComponent& light, class Shader& shader);



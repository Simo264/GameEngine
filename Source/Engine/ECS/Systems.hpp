#pragma once

#include "Core/Math/Math.hpp"

/* --------------------------------------------
			TransformComponent Systems
	-------------------------------------------- */
	

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



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

/* Update projection matrix */
void UpdateCameraFov(struct CameraComponent& camera, float fov);
void UpdateCameraAspect(struct CameraComponent& camera, float aspect);
void UpdateCameraZNear(struct CameraComponent& camera, float zNear);
void UpdateCameraZFar(struct CameraComponent& camera, float zFar);

/* Calculates the front vector from the Camera's (updated) Euler Angles */
void UpdateCameraVectors(struct CameraComponent& camera);
void MoveCamera(struct CameraComponent& camera, double deltaTime);
void RotateCamera(struct CameraComponent& camera, double deltaTime);

void ProcessCameraInput(struct CameraComponent& camera, double deltaTime);

/* --------------------------------------------
			Render Systems
	-------------------------------------------- */
void RenderMesh(struct StaticMeshComponent& mesh, const Mat4f& transform, class Shader& shader);
void RenderLight(struct DirLightComponent& light, class Shader& shader);
void RenderLight(struct PointLightComponent& light, class Shader& shader);
void RenderLight(struct SpotLightComponent& light, class Shader& shader);



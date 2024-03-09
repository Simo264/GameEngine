#pragma once

#include "Core/Core.hpp"
#include <entt/entt.hpp> /* Entity component system */

/* ----------------------------------------------------------------------------
	Scene class contains all of objects that are placed in world.	
	---------------------------------------------------------------------------- */
class Scene
{
public:
	Scene()
		: _primaryCamera{ entt::null } /* Invalid entity camera ID */
	{}

	void LoadScene(const Path& filepath);
	void SaveScene(const Path& filepath);

	void DrawScene(class Shader& shader);
	
	/* Remove all objects from scene */
	void ClearScene();

	/* Create object in scene */
	class GameObject CreateObject(const char* label, uint32_t type);

	/* Remove object from scene */
	void DestroyObject(class GameObject& object);

	/* Return the reference of registry */
	entt::registry& Reg() { return _registry; }

	void SetPrimaryCamera(class GameObject cameraObject);

	class GameObject GetPrimaryCamera();

private:
	/* We can create a entt::registry to store our entities */
	entt::registry _registry;
	
	entt::entity _primaryCamera; /* Entity ID */
};

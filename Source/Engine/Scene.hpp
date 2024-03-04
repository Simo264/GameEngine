#pragma once

#include "Core/Core.hpp"
#include <entt/entt.hpp> /* Entity component system */

/* ----------------------------------------------------------------------------
	Scene class contains all of objects that are placed in world.	
	---------------------------------------------------------------------------- */

class Scene
{
public:
	Scene() = default;

	void LoadScene(const Path& filepath);
	void SaveScene(const Path& filepath);

	void DrawScene(class Shader* shader);
	
	/* Remove all objects from scene */
	void ClearScene();

	/* Create object in scene */
	class GameObject CreateObject(const char* label, uint32_t type);

	/* Remove object from scene */
	void DestroyObject(class GameObject* object);

	/* Return the reference of registry */
	entt::registry& Reg() { return _registry; }

private:
	/* We can create a entt::registry to store our entities */
	entt::registry _registry;

#if 0
	SharedPointer<DirectionalLight> ParseDirectionalLight(
		IFStream& file, String& line, char name[64], char value[64]);
	
	SharedPointer<PointLight> ParsePointLight(
		IFStream& file, String& line, char name[64], char value[64]);
	
	//SharedPointer<SpotLight> ParseDirectionalLight();
	
	SharedPointer<StaticMesh> ParseStaticMesh(
		IFStream& file, String& line, char name[64], char value[64]);
#endif
};

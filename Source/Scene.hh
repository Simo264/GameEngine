#pragma once

#include "Core.hh"
#include "NonCopyable.hh"

class Scene : public NonCopyable
{
public:
	Scene() : directionalLight{ nullptr } {}
	~Scene() = default;

	void DrawScene(class Shader* shader);

	void AddStaticMesh(class StaticMesh* staticMesh);
	void AddPointLight(class PointLight* pointLight);

	class DirectionalLight* directionalLight;
	Vector<class StaticMesh*> meshes;
	Vector<class PointLight*>	pointLights;
};

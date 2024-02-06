#pragma once

#include "NonCopyable.hh"

#include "Lighting/DirectionalLight.hh"
#include "Lighting/PointLight.hh"
#include "Lighting/SpotLight.hh"

template <typename T>
struct SceneObject {
	T* object;
	bool visible;

	SceneObject() : object{nullptr}, visible{true}{}
	SceneObject(T* obj, bool b) : object{ obj }, visible{ b } {}
};

class Scene : public NonCopyable
{
public:
	Scene() : dLight{ nullptr, true } {}
	~Scene() = default;

	void DrawScene(class Shader* shader);
	void AddStaticMesh(class StaticMesh* staticMesh);
	void AddPointLight(PointLight* pointLight);
	
	SceneObject<DirectionalLight> dLight;
	Vector<SceneObject<StaticMesh>> sMeshes;
	Vector<SceneObject<PointLight>>	pLights;
};

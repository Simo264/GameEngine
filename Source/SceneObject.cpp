#include "SceneObject.hh"
#include "Lighting/DirectionalLight.hh"
#include "Lighting/PointLight.hh"
#include "Lighting/SpotLight.hh"
#include "Mesh/StaticMesh.hh"

/* Template specialization */
template<class T>
void SceneObject<T>::SetName()
{
	char str[32];
	sprintf_s(str, "Object_%d", objectID);
	name = str;
}

template<>
void SceneObject<DirectionalLight>::SetName()
{
	char str[32];
	sprintf_s(str, "Directional_light_%d", objectID);
	name = str;
}

template<>
void SceneObject<PointLight>::SetName()
{
	char str[32];
	sprintf_s(str, "Point_light_%d", objectID);
	name = str;
}

template<>
void SceneObject<StaticMesh>::SetName()
{
	char str[32];
	sprintf_s(str, "Mesh_%d", objectID);
	name = str;
}

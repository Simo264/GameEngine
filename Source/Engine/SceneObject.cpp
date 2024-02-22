#include "SceneObject.hpp"
#include "Engine/Lighting/DirectionalLight.hpp"
#include "Engine/Lighting/PointLight.hpp"
#include "Engine/Lighting/SpotLight.hpp"
#include "Engine/StaticMesh.hpp"


/* -----------------------------------------------------
 *          PUBLIC METHODS
 * -----------------------------------------------------
*/

SceneObject::SceneObject()
{
	visible = true;
	_instanceID = _id++;
	
	tagName = "Scene object";
}

bool SceneObject::Compare(const SceneObject& other) const
{
	return (this->GetID() == other.GetID());
}

/* -----------------------------------------------------
 *          PRIVATE METHODS
 * -----------------------------------------------------
*/


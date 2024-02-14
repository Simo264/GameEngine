#include "SceneObject.hh"
#include "Lighting/DirectionalLight.hh"
#include "Lighting/PointLight.hh"
#include "Lighting/SpotLight.hh"
#include "Mesh/StaticMesh.hh"


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


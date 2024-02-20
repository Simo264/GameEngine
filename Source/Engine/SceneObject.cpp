#include "SceneObject.hpp"
#include "Engine/Lighting/DirectionalLight.hpp"
#include "Engine/Lighting/PointLight.hpp"
#include "Engine/Lighting/SpotLight.hpp"
#include "Engine/Mesh/StaticMesh.hpp"


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

String SceneObject::ToString() const
{
	OStringStream oss;
	oss << "type=SceneObject\n";
	oss << "tag=" << tagName << "\n";
	oss << "id=" << GetID() << "\n";
	return oss.str();
}

/* -----------------------------------------------------
 *          PRIVATE METHODS
 * -----------------------------------------------------
*/


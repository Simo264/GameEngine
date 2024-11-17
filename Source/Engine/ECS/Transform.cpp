#include "Transform.hpp"

#include "Core/Math/Ext.hpp"

Transform::Transform()
	: position{ 0.0f, 0.0f, 0.0f },
		scale{ 1.0f, 1.0f, 1.0f },
		rotation{ 0.0f, 0.0f, 0.0f }
{
	UpdateTransformation();
}

void Transform::UpdateTransformation()
{
	mat4f translationMatrix = glm::translate(mat4f(1.0f), position);
	mat4f rotationMatrix = mat4f(quat(vec3f(glm::radians(rotation.x), glm::radians(rotation.y), glm::radians(rotation.z))));
	mat4f scalingMatrix = glm::scale(mat4f(1.0f), scale);
	_transformation = translationMatrix * rotationMatrix * scalingMatrix;
}
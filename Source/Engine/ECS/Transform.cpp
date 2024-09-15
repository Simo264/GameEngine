#include "Transform.hpp"

#include "Core/Math/Extensions.hpp"


Transform::Transform()
	: position{ 0.0f, 0.0f, 0.0f },
		scale{ 1.0f, 1.0f, 1.0f },
		rotation{ 0.0f, 0.0f, 0.0f }
{
	UpdateTransformation();
}

void Transform::UpdateTransformation()
{
	static const mat4f I = mat4f(1.0f);
	mat4f translationMatrix = Math::Translate(I, position);
	mat4f rotationMatrix = mat4f(quat(vec3f(Math::Radians(rotation.x), Math::Radians(rotation.y), glm::radians(rotation.z))));
	mat4f scalingMatrix = Math::Scale(I, scale);
	_transformation = translationMatrix * rotationMatrix * scalingMatrix;
}
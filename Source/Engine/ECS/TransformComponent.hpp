#pragma once

#include "Core/Math/Math.hpp"
#include "Engine/ECS/IComponent.hpp"

/* ----------------------------------------------------------------------
	This component defines the transformation (location, rotation, scale) 
	of an object in world
 ------------------------------------------------------------------------ */
class TransformComponent : public IComponent
{
public:
	TransformComponent() : 
		position{ 0,0,0 }, 
		scale{ 1,1,1 }, 
		rotation{ 0,0,0 }
	{}

	static const char* GetComponentName(bool lower = false)
	{
		if(lower)
			return "transformcomponent";

		return "TransformComponent";
	}

	/* Return following string representation: 
		"position=<position.x,position.y,position.z>"
		"scale=<scale.x,scale.y,scale.z>"
		"rotation=<rotation.x,rotation.y,rotation.z>"
	*/
	void ToString(String& out) const override
	{
		char buff[64]{};
		
		sprintf_s(buff, "position=%.3f,%.3f,%.3f\n", position.x, position.y, position.z);
		out.append(buff);
		
		sprintf_s(buff, "scale=%.3f,%.3f,%.3f\n", scale.x, scale.y, scale.z);
		out.append(buff);

		sprintf_s(buff, "rotation=%.3f,%.3f,%.3f\n", rotation.x, rotation.y, rotation.z);
		out.append(buff);
	}

	/* Calculate the model matrix */
	Mat4f GetTransformation() const
	{
		static const Mat4f I = Mat4f(1.0f);

		Mat4f translationMatrix = glm::translate(I, position);
		Mat4f rotationMatrix =
			glm::rotate(I, glm::radians(rotation.x), Vec3f(1.0f, 0.0f, 0.0f)) *
			glm::rotate(I, glm::radians(rotation.y), Vec3f(0.0f, 1.0f, 0.0f)) *
			glm::rotate(I, glm::radians(rotation.z), Vec3f(0.0f, 0.0f, 1.0f));
		Mat4f scalingMatrix = glm::scale(I, scale);

		return translationMatrix * rotationMatrix * scalingMatrix;
	}

	Vec3f position;
	Vec3f scale;
	Vec3f rotation;	/* in degrees */
};
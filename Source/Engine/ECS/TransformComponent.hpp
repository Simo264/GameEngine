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
		degrees{ 0,0,0 }
	{}

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

		sprintf_s(buff, "rotation=%.3f,%.3f,%.3f\n", degrees.x, degrees.y, degrees.z);
		out.append(buff);
	}

	/* Calculate the model matrix */
	Mat4f GetTransformation() const
	{
		static const Mat4f I = Mat4f(1.0f);

		Mat4f translation = glm::translate(I, position);
		Mat4f rotation =
			glm::rotate(I, glm::radians(degrees.x), Vec3f(1.0f, 0.0f, 0.0f)) *
			glm::rotate(I, glm::radians(degrees.y), Vec3f(0.0f, 1.0f, 0.0f)) *
			glm::rotate(I, glm::radians(degrees.z), Vec3f(0.0f, 0.0f, 1.0f));
		Mat4f scaling = glm::scale(I, scale);

		return translation * rotation * scaling;
	}

	Vec3f position;
	Vec3f scale;
	Vec3f degrees;	/* rotation in degree */
};
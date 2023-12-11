#pragma once

#include "../Core.hh"
#include "../Graphics/VertexArray.hh"
#include "../Graphics/Shader.hh"

class Cube
{
public:
	Cube();
	~Cube() = default;

	void Draw(Graphics::Shader& shader);
	void Destroy();

	vec3f position;
	vec3f scaling;
	vec3f rotation;
	float rotationAngle;

	Graphics::VertexArray vertexArray;
	uint32_t instanceBuffer;

private:
	mat4f _model;
};


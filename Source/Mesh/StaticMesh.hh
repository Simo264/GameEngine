#pragma once

#include "../Core.hh"
#include "../RootComponent.hh"
#include "../Texture2D.hh"
#include "../Graphics/VertexArray.hh"
#include "../Graphics/Shader.hh"


// A Static Mesh is a piece of geometry that consists of a set of polygons that can be and rendered by the graphics card. 
// Static Meshes can be translated, rotated, and scaled but they cannot have their vertices animated in any way.
// These are 3D models created in external modeling applications (such as 3dsMax, Maya, Softimage, and so on) that are imported.
class StaticMesh : public RootComponent
{
public:
	StaticMesh() : RootComponent(), diffuse{nullptr}, specular{nullptr} {}
	~StaticMesh() = default;

	StaticMesh(const StaticMesh&) = delete;            // delete copy constructor
	StaticMesh& operator=(const StaticMesh&) = delete; // delete assign op

	void InitMesh(VAData& data, VAConfig& config);
	void DestroyMesh();
	void Draw(Shader* shader);

	VertexArray vertexArray;
	Texture2D*	diffuse;
	Texture2D*	specular;
	//Texture2D*	normal;
};


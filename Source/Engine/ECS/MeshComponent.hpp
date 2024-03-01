#pragma once

#include "Core/Core.hpp"
#include "Engine/Graphics/VertexArray.hpp"
#include "Engine/ECS/TransformComponent.hpp"
#include "Engine/Material.hpp"

class MeshComponent
{
public:
	/*	Create empty vertex array object. The are two ways to initialize mesh's vao:
			1. by using ObjectLoader:
				ObjectLoader loader(PathToObjectFile);
				loader.LoadMesh(mesh);
			
			2. by using the MeshComponent method InitVAO:
				VertexBufferLayout layout;
				layout.PushAttributes({ ... }); 

				VertexBufferData data;
				data.vertexDataSize = ...;
				data.vertextDataPtr = ...;
				data.indexDataSize	= ...;
				data.indexDataPtr		= ...;
				mesh->InitVAO(layout, data);
	*/
	MeshComponent();
	
	/* Free resources from GPU */
	~MeshComponent() 
	{ 
		vertexArray->Destroy(); 
	}

	void InitVAO(const VertexBufferLayout& layout, const VertexBufferData& data) const;

	/* Draw mesh */
	void Draw(const TransformComponent& tranform, class Shader* shader) const;

	/* By default it does not contain textures */
	Material material;

	SharedPointer<VertexArray> vertexArray;
};

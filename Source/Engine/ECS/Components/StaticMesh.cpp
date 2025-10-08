#include "StaticMesh.hpp"

#include "Utils/Logger.hpp"
#include "Engine/Graphics/Shader.hpp"

namespace Components
{
	void StaticMesh::Create()
	{
		vertexArray.Create();
		vertexBuffer.Create();
		indexBuffer.Create();
	}
	
	void StaticMesh::Release()
	{
		vertexArray.Release();
		vertexBuffer.Release();
		indexBuffer.Release();
	}

	void StaticMesh::UpdateVertexData(u32 offset, u32 size, const void* data) const
	{
		if ((vertexBuffer.GetStorageFlags() & BufferStorageFlags::DYNAMIC_STORAGE) == BufferStorageFlags::DYNAMIC_STORAGE)
			vertexBuffer.UpdateStorage(offset, size, data);
		else
			CONSOLE_WARN("WARNING: Attempted to update a StaticMesh with non-dynamic vertex data. This operation is not allowed.");
	}

	void StaticMesh::Copy(StaticMesh& other) const
	{
		if (!vertexArray.Valid())
		{
			CONSOLE_WARN("Source mesh is not valid. Can't perform copy operation.");
			return;
		}
		if (!other.vertexArray.Valid())
		{
			CONSOLE_WARN("Destination mesh is not valid. Can't perform copy operation.");
			return;
		}
		
		auto& readVertBuffer = vertexBuffer;
		auto bufferSize = readVertBuffer.GetBufferSize();
		if (bufferSize == 0)
		{
			CONSOLE_WARN("Vertex buffer is empty. Can't perform copy operation.");
			return;
		}
		other.numVertices = numVertices;
		other.numIndices = numIndices;
		
		// Copy vertex buffer 
		other.vertexBuffer.CreateImmutableStorage(bufferSize, nullptr, vertexBuffer.GetStorageFlags());
		other.vertexBuffer.CopyFrom(readVertBuffer.id, 0, 0, bufferSize);
		other.vertexArray.AttachVertexBuffer(0, other.vertexBuffer.id, 0, sizeof(Vertex));
		

		// Copy index buffer
		auto& readIndexBuffer = indexBuffer;
		bufferSize = readIndexBuffer.GetBufferSize();
		if (bufferSize > 0)
		{
			other.indexBuffer.CreateImmutableStorage(bufferSize, nullptr, indexBuffer.GetStorageFlags());
			other.indexBuffer.CopyFrom(readIndexBuffer.id, 0, 0, bufferSize);
			other.vertexArray.AttachIndexBuffer(other.indexBuffer.id);
		}
	}
};


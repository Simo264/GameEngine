#pragma once

#include "../Core.hh"
#include "VertexArray.hh"

namespace Graphics
{
	class Renderer
	{
	public:
		Renderer() = delete;
		~Renderer() = delete;
		
		static void DrawIndexed(VertexArray& vertexArray);
		static void DrawIndexedInstancing(VertexArray& vertexArray, int nInstances);
	
		static uint32_t numRenderCallsPerFrame;
	};
}
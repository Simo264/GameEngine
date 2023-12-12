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
		
		static void DrawArrays(VertexArray& vertexArray);
		static void DrawArraysInstanced(VertexArray& vertexArray, int nInstances);

		static void DrawIndexed(VertexArray& vertexArray);
		static void DrawIndexedInstanced(VertexArray& vertexArray, int nInstances);
	
		static uint32_t numRenderCallsPerFrame;
	};
}
#pragma once

#include "Core.hh"


// Static class Renderer:
// takes care of calling the drawing functions
// --------------------------------------------
class Renderer
{
public:
	Renderer()	= delete;
	~Renderer() = delete;
		
	static void DrawArrays(class VertexArray* vertexArray);
	static void DrawArraysInstanced(class VertexArray* vertexArray, int nInstances);

	static void DrawIndexed(class VertexArray* vertexArray);
	static void DrawIndexedInstanced(class VertexArray* vertexArray, int nInstances);
	
	inline static uint32_t drawCalls = 0;
};

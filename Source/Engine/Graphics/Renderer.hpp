#pragma once

#include "Core.hpp"

/* -----------------------------------
	Static class Renderer:  
	takes care of calling the drawing functions
	----------------------------------- */
class Renderer
{
public:
	Renderer()	= delete;
	~Renderer() = delete;
	
	/* Render calls with no indices */
	static void DrawArrays(class VertexArray* vertexArray);
	static void DrawArraysInstanced(class VertexArray* vertexArray, int nInstances);

	/* Render calls with indices */
	static void DrawIndexed(class VertexArray* vertexArray);
	static void DrawIndexedInstanced(class VertexArray* vertexArray, int nInstances);
	
	inline static uint32_t drawCalls = 0;
	inline static uint32_t drawMode = GL_TRIANGLES;
};

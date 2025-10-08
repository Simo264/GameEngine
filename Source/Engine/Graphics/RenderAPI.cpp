#include "RenderAPI.hpp"

#include "Utils/Logger.hpp"
#include "Engine/Globals.hpp"
#include "Engine/ECS/Scene.hpp"

#include <glad/gl.h>

namespace RenderAPI
{
	// ===================================
	//				Stencil test
	// ===================================

	void EnableStencilTest()
	{
		glEnable(GL_STENCIL_TEST);
	}

	void DisableStencilTest()
	{
		glDisable(GL_STENCIL_TEST);
	}

	void SetStencilFun(CompareFunc fun, i32 ref, i32 mask)
	{
		glStencilFunc(static_cast<u32>(fun), ref, mask);
	}

	void SetStencilOp(StencilOpMode sfail, StencilOpMode dpfail, StencilOpMode dppass)
	{
		glStencilOp(static_cast<i32>(sfail), static_cast<i32>(dpfail), static_cast<i32>(dppass));
	}

	void SetStencilMask(i32 mask)
	{
		glStencilMask(mask);
	}

	void EnableWritingStencilBuffer()
	{
		SetStencilMask(0xFF);
	}

	void DisableWritingStencilBuffer()
	{
		SetStencilMask(0x00);
	}

	// ===================================
	//				Depth test
	// ===================================

	void EnableDepthTest()
	{
		glEnable(GL_DEPTH_TEST);
	}

	void DisableDepthTest()
	{
		glDisable(GL_DEPTH_TEST);
	}

	void EnableWritingDepthBuffer()
	{
		glDepthMask(GL_TRUE);
	}

	void DisableWritingDepthBuffer()
	{
		glDepthMask(GL_FALSE);
	}

	void SetDepthFun(CompareFunc fun)
	{
		glDepthFunc(static_cast<u32>(fun));
	}

	// ===================================
	//				Face culling 
	// ===================================

	void EnableFaceCulling()
	{
		glEnable(GL_CULL_FACE);
	}

	void DisableFaceCulling()
	{
		glDisable(GL_CULL_FACE);
	}

	void SetCullFace(CullFaceMode mode)
	{
		glCullFace(static_cast<u32>(mode));
	}

	void SetFrontFace(FrontFaceMode mode)
	{
		glFrontFace(static_cast<u32>(mode));
	}

	// ===================================
	//				Draw commands
	// ===================================

	void DrawArrays(RenderMode mode, const VertexArray& vertexArray, u32 numVertices)
	{
		if (numVertices == 0)
		{
			CONSOLE_WARN("Invalid vertices number!");
			return;
		}

		vertexArray.Bind();
		glDrawArrays(static_cast<u32>(mode), 0, numVertices);
		g_DrawCalls++;
	}
	
	void DrawElements(RenderMode mode, const VertexArray& vertexArray, u32 numIndices)
	{
		if (numIndices == 0)
		{
			CONSOLE_WARN("Invalid indices number!");
			return;
		}

		vertexArray.Bind();
		glDrawElements(static_cast<u32>(mode), numIndices, GL_UNSIGNED_INT, 0);
		g_DrawCalls++;
	}

	void Draw(RenderMode mode, const Components::StaticMesh& mesh)
	{
		if (mesh.numIndices > 0)
			RenderAPI::DrawElements(RenderMode::TRIANGLES, mesh.vertexArray, mesh.numIndices);
		else
			RenderAPI::DrawArrays(RenderMode::TRIANGLES, mesh.vertexArray, mesh.numVertices);
	}
}
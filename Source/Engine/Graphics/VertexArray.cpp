#include "VertexArray.hpp"
#include "Core/Log/Logger.hpp"

#include "Core/Platform/OpenGL/OpenGL.hpp"

/* -----------------------------------------------------
 *          PUBLIC METHODS
 * -----------------------------------------------------
*/

void VertexArray::Create()
{
  glCreateVertexArrays(1, &vao);
}

void VertexArray::Delete() const
{
  glDeleteVertexArrays(1, &vao);
}

void VertexArray::Bind() const
{
  glBindVertexArray(vao);
}

void VertexArray::Unbind() const
{
  glBindVertexArray(0);
}

void VertexArray::EnableAttribute(int attribindex) const
{
  glEnableVertexArrayAttrib(vao, attribindex);
}

void VertexArray::DisableAttribute(int attribindex) const
{
  glDisableVertexArrayAttrib(vao, attribindex);
}

void VertexArray::BindVertexBuffer(int bindingindex, const VertexBuffer& vertexBuffer, int offset, int stride) const
{
  glVertexArrayVertexBuffer(vao, bindingindex, vertexBuffer.bufferID, offset, stride);
}

void VertexArray::SetAttribFormat(int attribindex, int size, int type, bool normalize, int relativeoffset) const
{
  glVertexArrayAttribFormat(vao, attribindex, size, type, normalize, relativeoffset);
}

void VertexArray::SetAttribBinding(int attribindex, int bindingindex) const
{
  glVertexArrayAttribBinding(vao, attribindex, bindingindex);
}

/* -----------------------------------------------------
 *          PRIVATE METHODS
 * -----------------------------------------------------
*/

#include "VertexArray.hpp"
#include "Core/Log/Logger.hpp"

#include "Core/Platform/OpenGL/OpenGL.hpp"

/* -----------------------------------------------------
 *          PUBLIC METHODS
 * -----------------------------------------------------
*/

VertexArray::VertexArray()
  : id{ static_cast<uint32_t>(-1) },
    numIndices{ 0 },
    numVertices{ 0 },
    eboAttachment{},
    vboAttachments{}
{}

void VertexArray::Create()
{
  glCreateVertexArrays(1, &id);
}

void VertexArray::Delete()
{
  glDeleteVertexArrays(1, &id);
  
  if (eboAttachment.IsValid())
    eboAttachment.Delete();

  for (VertexBuffer& vbo : vboAttachments)
    if (vbo.IsValid())
      vbo.Delete();

  id = static_cast<uint32_t>(-1);

  vboAttachments.clear();
}

void VertexArray::Bind() const
{
  glBindVertexArray(id);
}

void VertexArray::Unbind() const
{
  glBindVertexArray(0);
}

void VertexArray::EnableAttribute(int attribindex) const
{
  glEnableVertexArrayAttrib(id, attribindex);
}

void VertexArray::DisableAttribute(int attribindex) const
{
  glDisableVertexArrayAttrib(id, attribindex);
}

void VertexArray::AttachVertexBuffer(int bindingindex, VertexBuffer& buffer, int offset, int stride)
{
  vboAttachments.push_back(buffer);
  glVertexArrayVertexBuffer(id, bindingindex, buffer.id, offset, stride);
}

void VertexArray::AttachElementBuffer(ElementBuffer& buffer)
{
  eboAttachment = buffer;
  glVertexArrayElementBuffer(id, buffer.id);
}

void VertexArray::SetAttribFormat(int attribindex, int size, int type, bool normalize, int relativeoffset) const
{
  glVertexArrayAttribFormat(id, attribindex, size, type, normalize, relativeoffset);
}

void VertexArray::SetAttribBinding(int attribindex, int bindingindex) const
{
  glVertexArrayAttribBinding(id, attribindex, bindingindex);
}

void VertexArray::SetVertexSpecifications(const VertexSpecifications& specs) const
{
  SetAttribFormat(specs.attrindex, specs.components, specs.type, specs.normalized, specs.relativeoffset);
  
  SetAttribBinding(specs.attrindex, specs.bindingindex);
  
  EnableAttribute(specs.attrindex);
}

/* -----------------------------------------------------
 *          PRIVATE METHODS
 * -----------------------------------------------------
*/

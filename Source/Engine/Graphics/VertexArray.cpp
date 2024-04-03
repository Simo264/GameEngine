#include "VertexArray.hpp"
#include "Core/Log/Logger.hpp"

#include "Core/Platform/OpenGL/OpenGL.hpp"

/* -----------------------------------------------------
 *          PUBLIC METHODS
 * -----------------------------------------------------
*/

void VertexArray::Create()
{
  glCreateVertexArrays(1, &id);
}

void VertexArray::Delete() const
{
  glDeleteVertexArrays(1, &id);
  
  for (const VertexBuffer& vb : vbosAttached)
    if(vb.IsValid())
      vb.Delete();  

  if (eboAttached.IsValid())
    eboAttached.Delete();
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

void VertexArray::AttachVertexBuffer(int bindingindex, const VertexBuffer& buffer, int offset, int stride)
{
  const auto end = vbosAttached.end();
  auto it = std::find_if(vbosAttached.begin(), end, [&](const VertexBuffer& attached) {
    return attached.Compare(buffer);
    });
  if (it != end)
  {
    CONSOLE_WARN("Vertex buffer is already attached to this vertex array object!");
    return;
  }

  vbosAttached.push_back(buffer);
  glVertexArrayVertexBuffer(id, bindingindex, buffer.id, offset, stride);
}

void VertexArray::AttachElementBuffer(const ElementBuffer& buffer)
{
  if (eboAttached.IsValid())
  {
    CONSOLE_WARN("Element buffer is already attached to this vertex array object!");
    return;
  }

  eboAttached = buffer;
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

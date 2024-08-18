#include "VertexArray.hpp"

#include "Core/OpenGL.hpp"
#include "Core/Log/Logger.hpp"

VertexArray::VertexArray()
  : id{ 0 },
    numIndices{ 0 },
    numVertices{ 0 },
    eboAttachmentID{ 0 },
    vboAttachmentIDs{}
{}

void VertexArray::Create()
{
  glCreateVertexArrays(1, &id);
}

void VertexArray::Delete()
{
  if (vboAttachmentIDs.size() > 0 || eboAttachmentID != 0)
  {
    if (eboAttachmentID != 0)
      vboAttachmentIDs.push_back(eboAttachmentID);
    
    glDeleteBuffers(vboAttachmentIDs.size(), vboAttachmentIDs.data());
  }
  
  /* Destroy Vector */
  Vector<u32>().swap(vboAttachmentIDs);
  eboAttachmentID = 0;

  glDeleteVertexArrays(1, &id);
  id = 0;
}

void VertexArray::Bind() const
{
  glBindVertexArray(id);
}

void VertexArray::Unbind() const
{
  glBindVertexArray(0);
}

void VertexArray::EnableAttribute(i32 attribindex) const
{
  glEnableVertexArrayAttrib(id, attribindex);
}

void VertexArray::DisableAttribute(i32 attribindex) const
{
  glDisableVertexArrayAttrib(id, attribindex);
}

void VertexArray::AttachVertexBuffer(i32 bindingindex, u32 bufferID, i32 offset, i32 stride)
{
  vboAttachmentIDs.push_back(bufferID);
  glVertexArrayVertexBuffer(id, bindingindex, bufferID, offset, stride);
}

void VertexArray::AttachElementBuffer(u32 bufferID)
{
  eboAttachmentID = bufferID;
  glVertexArrayElementBuffer(id, bufferID);
}

void VertexArray::SetAttribFormat(i32 attribindex, i32 size, i32 type, bool normalize, i32 relativeoffset) const
{
  glVertexArrayAttribFormat(id, attribindex, size, type, normalize, relativeoffset);
}

void VertexArray::SetAttribBinding(i32 attribindex, i32 bindingindex) const
{
  glVertexArrayAttribBinding(id, attribindex, bindingindex);
}

void VertexArray::SetBindingDivisor(i32 bindingindex, i32 divisor) const
{
  glVertexArrayBindingDivisor(id, bindingindex, divisor);
}

void VertexArray::SetVertexSpecifications(const VertexSpecifications& specs) const
{
  SetAttribFormat(specs.attrindex, specs.components, specs.type, specs.normalized, specs.relativeoffset);
  SetAttribBinding(specs.attrindex, specs.bindingindex);
  EnableAttribute(specs.attrindex);
}

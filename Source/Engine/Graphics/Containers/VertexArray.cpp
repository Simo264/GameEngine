#include "VertexArray.hpp"

#include "Core/OpenGL.hpp"
#include "Core/Log/Logger.hpp"

VertexArray::VertexArray()
  : id{ static_cast<uint32_t>(-1) },
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
  
  /* Destroy vector */
  vector<uint32_t>().swap(vboAttachmentIDs);
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

void VertexArray::EnableAttribute(int attribindex) const
{
  glEnableVertexArrayAttrib(id, attribindex);
}

void VertexArray::DisableAttribute(int attribindex) const
{
  glDisableVertexArrayAttrib(id, attribindex);
}

void VertexArray::AttachVertexBuffer(int bindingindex, uint32_t bufferID, int offset, int stride)
{
  vboAttachmentIDs.push_back(bufferID);
  glVertexArrayVertexBuffer(id, bindingindex, bufferID, offset, stride);
}

void VertexArray::AttachElementBuffer(uint32_t bufferID)
{
  eboAttachmentID = bufferID;
  glVertexArrayElementBuffer(id, bufferID);
}

void VertexArray::SetAttribFormat(int attribindex, int size, int type, bool normalize, int relativeoffset) const
{
  glVertexArrayAttribFormat(id, attribindex, size, type, normalize, relativeoffset);
}

void VertexArray::SetAttribBinding(int attribindex, int bindingindex) const
{
  glVertexArrayAttribBinding(id, attribindex, bindingindex);
}

void VertexArray::SetBindingDivisor(int bindingindex, int divisor) const
{
  glVertexArrayBindingDivisor(id, bindingindex, divisor);
}

void VertexArray::SetVertexSpecifications(const VertexSpecifications& specs) const
{
  SetAttribFormat(specs.attrindex, specs.components, specs.type, specs.normalized, specs.relativeoffset);
  SetAttribBinding(specs.attrindex, specs.bindingindex);
  EnableAttribute(specs.attrindex);
}

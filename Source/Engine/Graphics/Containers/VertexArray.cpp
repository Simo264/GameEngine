#include "VertexArray.hpp"

#include "Core/GL.hpp"
#include "Core/Log/Logger.hpp"

void VertexArray::Create()
{
  glCreateVertexArrays(1, &id);
}

void VertexArray::Delete()
{
  i32 numVBOAttached = GetNumVBOAttached();
  if (numVBOAttached > 0)
  {
    u32 bufferIDs[MAX_NUM_VBO_ATTACHMENTS + 1]{};
    i32 size = 0;
    
    for (i32 i = 0; i < numVBOAttached; i++, size++)
      bufferIDs[i] = vboAttachmentIDs[i];

    if (eboAttachmentID != 0)
      bufferIDs[size++] = eboAttachmentID;
    
    glDeleteBuffers(size, bufferIDs);
  }
  glDeleteVertexArrays(1, &id);

  id = 0;
  numVertices = 0;
  numIndices = 0;
  eboAttachmentID = 0;
  std::fill_n(vboAttachmentIDs, MAX_NUM_VBO_ATTACHMENTS, 0);
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
  vboAttachmentIDs[GetNumVBOAttached()] = bufferID;
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

i32 VertexArray::GetNumVBOAttached() const
{
  static auto countNonZero = [](u32 id) { return id != 0; };
  return std::count_if(std::begin(vboAttachmentIDs), std::end(vboAttachmentIDs), countNonZero);
}

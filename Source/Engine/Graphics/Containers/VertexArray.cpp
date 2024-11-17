#include "VertexArray.hpp"

#include "Core/GL.hpp"
#include "Core/Log/Logger.hpp"

void VertexArray::Create()
{
  glCreateVertexArrays(1, &id);
}

void VertexArray::Delete()
{
  if (vboAttachments.size() > 0)
  {
    u32 bufferIDs[MAX_NUM_VBO_ATTACHMENTS + 1]{};
    i32 size = 0;
    
    for (const Buffer& buffer : vboAttachments)
      if(buffer.IsValid())
        bufferIDs[size++] = buffer.id;
    
    if (eboAttachment.IsValid())
      bufferIDs[size++] = eboAttachment.id;
    
    glDeleteBuffers(size, bufferIDs);
  }
  glDeleteVertexArrays(1, &id);

  id = 0;
  numVertices = 0;
  numIndices = 0;
  eboAttachment = Buffer();
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

bool VertexArray::IsValid() const
{
  return (id != 0) && (glIsVertexArray(id) == GL_TRUE);
}

void VertexArray::EnableAttribute(i32 attribindex) const
{
  glEnableVertexArrayAttrib(id, attribindex);
}

void VertexArray::DisableAttribute(i32 attribindex) const
{
  glDisableVertexArrayAttrib(id, attribindex);
}

void VertexArray::AttachVertexBuffer(i32 bindingindex, Buffer buffer, i32 offset, i32 stride)
{
  vboAttachments.push_back(buffer);
  glVertexArrayVertexBuffer(id, bindingindex, buffer.id, offset, stride);
}

void VertexArray::AttachElementBuffer(Buffer buffer)
{
  eboAttachment = buffer;
  glVertexArrayElementBuffer(id, buffer.id);
}

void VertexArray::SetAttribFormatFLoat(i32 attribindex, i32 size, i32 type, bool normalize, i32 relativeoffset) const
{
  glVertexArrayAttribFormat(id, attribindex, size, type, normalize, relativeoffset);
}

void VertexArray::SetAttribFormatInteger(i32 attribindex, i32 size, i32 type, i32 relativeoffset) const
{
  glVertexArrayAttribIFormat(id, attribindex, size, type, relativeoffset);
}

void VertexArray::SetAttribFormatLong(i32 attribindex, i32 size, i32 relativeoffset) const
{
  glVertexArrayAttribLFormat(id, attribindex, size, GL_DOUBLE, relativeoffset);
}

void VertexArray::SetAttribBinding(i32 attribindex, i32 bindingindex) const
{
  glVertexArrayAttribBinding(id, attribindex, bindingindex);
}

void VertexArray::SetBindingDivisor(i32 bindingindex, i32 divisor) const
{
  glVertexArrayBindingDivisor(id, bindingindex, divisor);
}

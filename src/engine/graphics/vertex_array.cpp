#include "VertexArray.hpp"
#include "Utils/Logger.hpp"
#include <glad/gl.h>

VertexArray::~VertexArray()
{
  Release();
}

VertexArray& VertexArray::operator=(VertexArray&& other) noexcept
{
  if (this != &other)
  {
    Release();

    id = other.id;
    other.id = INVALID_VERTEXARRAY_ID;
  }
  return *this;
}

void VertexArray::DeleteArrays(u32 count, const VertexArrayId* ids)
{
  glDeleteVertexArrays(count, ids);
}

void VertexArray::Create()
{
  glCreateVertexArrays(1, &id);
}

void VertexArray::Release()
{
  if (Valid())
  {
    glCreateVertexArrays(1, &id);
    id = INVALID_VERTEXARRAY_ID;
  }
}

void VertexArray::Bind() const
{
  glBindVertexArray(id);
}

void VertexArray::EnableVertexAttribute(i32 attribindex) const
{
  glEnableVertexArrayAttrib(id, attribindex);
}

void VertexArray::DisableVertexAttribute(i32 attribindex) const
{
  glDisableVertexArrayAttrib(id, attribindex);
}

void VertexArray::AttachVertexBuffer(i32 bindingindex, BufferId bufferId, i32 offset, i32 stride) const
{
  glVertexArrayVertexBuffer(id, bindingindex, bufferId, offset, stride);
}

void VertexArray::AttachIndexBuffer(BufferId bufferId) const
{
  glVertexArrayElementBuffer(id, bufferId);
}

void VertexArray::SetAttribFormatFloat32(i32 attribindex, i32 size, VertexAttribType type, bool normalize, i32 relativeoffset) const
{
  glVertexArrayAttribFormat(id, attribindex, size, static_cast<u32>(type), normalize, relativeoffset);
}

void VertexArray::SetAttribFormatFloat64(i32 attribindex, i32 size, i32 relativeoffset) const
{
  glVertexArrayAttribLFormat(id, attribindex, size, GL_DOUBLE, relativeoffset);
}

void VertexArray::SetAttribFormatInteger32(i32 attribindex, i32 size, VertexAttribType type, i32 relativeoffset) const
{
  glVertexArrayAttribIFormat(id, attribindex, size, static_cast<u32>(type), relativeoffset);
}

void VertexArray::SetupVertexAttribute(i32 attribindex, i32 bindingindex, VertexFormat format) const
{
  EnableVertexAttribute(attribindex);
  SetAttribBinding(attribindex, bindingindex);

  switch (format.type)
  {
    // Floating-point and normalized integer types
    case VertexAttribType::BYTE:
    case VertexAttribType::UNSIGNED_BYTE:
    case VertexAttribType::SHORT:
    case VertexAttribType::UNSIGNED_SHORT:
    case VertexAttribType::FLOAT:
    case VertexAttribType::HALF_FLOAT:
      SetAttribFormatFloat32(attribindex, format.size, format.type, format.normalized, format.relativeoffset);
      break;

    // Pure integer types (no normalization)
    case VertexAttribType::INT:
    case VertexAttribType::UNSIGNED_INT:
      SetAttribFormatInteger32(attribindex, format.size, format.type, format.relativeoffset);
      break;
    
      // Double precision floating-point
    case VertexAttribType::DOUBLE:
      SetAttribFormatFloat64(attribindex, format.size, format.relativeoffset);
      break;

    default:
      // Fallback to float format
      SetAttribFormatFloat32(attribindex, format.size, format.type, format.normalized, format.relativeoffset);
      break;
  }
}

void VertexArray::SetAttribBinding(i32 attribindex, i32 bindingindex) const
{
  glVertexArrayAttribBinding(id, attribindex, bindingindex);
}

void VertexArray::SetBindingDivisor(i32 bindingindex, i32 divisor) const
{
  glVertexArrayBindingDivisor(id, bindingindex, divisor);
}

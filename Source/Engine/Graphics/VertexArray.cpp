#include "VertexArray.hpp"
#include "Core/Log/Logger.hpp"

#include "Core/Platform/OpenGL/OpenGL.hpp"

/* -----------------------------------------------------
 *          PUBLIC METHODS
 * -----------------------------------------------------
*/

VertexArray::VertexArray()
{
  Generate();
}

void VertexArray::Generate()
{
  glGenVertexArrays(1, &_vao);
}

void VertexArray::Delete() const
{
  glDeleteVertexArrays(1, &_vao);
}

void VertexArray::Destroy() const
{
  glDeleteVertexArrays(1, &_vao);

  _vbo.Delete();
  _ebo.Delete();
}

void VertexArray::Bind() const
{
  glBindVertexArray(_vao);
}

void VertexArray::Unbind() const
{
  glBindVertexArray(0);
}

void VertexArray::EnableAttribute(int index) const
{
  //glEnableVertexArrayAttrib(_vao, index);

  Bind();
    glEnableVertexAttribArray(index);
  Unbind();
}

void VertexArray::DisableAttribute(int index) const
{
  //glDisableVertexArrayAttrib(_vao, index);

  Bind();
    glDisableVertexAttribArray(index);
  Unbind();
}

void VertexArray::BindVertexBuffer(const VertexBuffer& vertexBuffer, int bindingindex, int offset, int stride) const
{
  //glVertexArrayVertexBuffer(_vao, bindingindex, vertexBuffer.GetBufferID(), offset, stride);

  Bind();
    glBindVertexBuffer(bindingindex, vertexBuffer.GetBufferID(), offset, stride);
  Unbind();
}

void VertexArray::SetAttribSpecification(int attribindex, int size, int type, bool normalize, int relativeoffset) const
{
  //glVertexArrayAttribFormat(_vao, attribindex, size, GL_FLOAT, GL_FALSE, relativeoffset);

  Bind();
    glVertexAttribFormat(attribindex, size, type, normalize, relativeoffset);
  Unbind();
}

void VertexArray::SetAttribBinding(int attribindex, int bindingindex) const
{
  //glVertexArrayAttribBinding(_vao, attribindex, bindingindex);

  Bind();
    glVertexAttribBinding(attribindex, bindingindex);
  Unbind();
}

#if 0
VertexArray::VertexArray(
  const VertexBufferLayout& layout, const VertexBufferData& data, uint32_t usage)
  : _vao{ 0 },
    _vbo{ 0 },
    _ebo{ 0 },
    numVertices{ 0 },
    numIndices{ 0 },
    vboSize{ 0 }
{
  glGenVertexArrays(1, &_vao);
  glGenBuffers(1, &_vbo);
  glGenBuffers(1, &_ebo);
  InitializeBuffers(layout, data, usage);
}

void VertexArray::InitializeBuffers(const VertexBufferLayout& layout, const VertexBufferData& data, uint32_t usage)
{
  _layout = layout;
  vboSize = data.vertexDataSize;

  /* Bind current VAO object */
  glBindVertexArray(_vao);

  /* Initialize vertex buffer (mutable storage) */
  glBindBuffer(GL_ARRAY_BUFFER, _vbo);
  glBufferData(GL_ARRAY_BUFFER, data.vertexDataSize, data.vertextDataPtr, usage);

  /* Initialize index buffer (mutable storage) */
  if (data.indexDataSize > 0)
  {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.indexDataSize, data.indexDataPtr, usage);
    numIndices = data.indexDataSize / sizeof(uint32_t);
  }

  int stride = std::accumulate(layout.attributes.begin(), layout.attributes.end(), 0);
  for (uint32_t i = 0, offset = 0; i < layout.numAttrs; i++)
  {
    /* Set vertex buffer layout */
    glVertexAttribPointer(i, layout.attributes[i], GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)offset);
    glEnableVertexAttribArray(i);
    offset += layout.attributes[i] * sizeof(float);
  }
  numVertices = data.vertexDataSize / (stride * sizeof(float));

  glBindVertexArray(0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

#endif

/* -----------------------------------------------------
 *          PRIVATE METHODS
 * -----------------------------------------------------
*/

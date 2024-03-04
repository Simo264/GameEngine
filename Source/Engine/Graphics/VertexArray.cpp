#include "VertexArray.hpp"
#include "Core/Log/Logger.hpp"

#include "Core/Platform/OpenGL/OpenGL.hpp"

/* -----------------------------------------------------
 *          PUBLIC METHODS
 * -----------------------------------------------------
*/

VertexArray::VertexArray()
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
}

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

void VertexArray::InitializeBuffers(
  const VertexBufferLayout& layout, const VertexBufferData& data, uint32_t usage)
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

void VertexArray::Bind() const
{ 
  glBindVertexArray(_vao); 
};

void VertexArray::Unbind() const
{ 
  glBindVertexArray(0); 
};

void VertexArray::CopyVertexBufferData(uint32_t writeBuffer)
{
  if (vboSize == 0)
  {
    CONSOLE_WARN("Vertex buffer is empty");
    return;
  }

  glBindBuffer(GL_COPY_READ_BUFFER, _vbo);
  glBindBuffer(GL_COPY_WRITE_BUFFER, writeBuffer);
  glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, vboSize);
  glBindBuffer(GL_COPY_READ_BUFFER, 0);
  glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
}

void VertexArray::CopyIndexBufferData(uint32_t writeBuffer)
{
  uint32_t indexBufferSize = numIndices * sizeof(uint32_t);
  if (indexBufferSize == 0)
  {
    CONSOLE_WARN("Index buffer is empty");
    return;
  }

  glBindBuffer(GL_COPY_READ_BUFFER, _ebo);
  glBindBuffer(GL_COPY_WRITE_BUFFER, writeBuffer);
  glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, indexBufferSize);
  glBindBuffer(GL_COPY_READ_BUFFER, 0);
  glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
}

void VertexArray::Destroy() const
{
  glDeleteVertexArrays(1, &_vao);
  glDeleteBuffers(1, &_vbo);
  glDeleteBuffers(1, &_ebo);
}

/* -----------------------------------------------------
 *          PRIVATE METHODS
 * -----------------------------------------------------
*/

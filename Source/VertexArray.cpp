#include "VertexArray.hh"
#include "Logger.hh"

/* -----------------------------------------------------
 *          VertexArrayConfig class
 * -----------------------------------------------------
*/

VertexArrayConfig::VertexArrayConfig()
{
  numAttrs = 0;
  layout.fill(0);
}

void VertexArrayConfig::PushAttribute(uint8_t attribute)
{
  layout[numAttrs++] = attribute;
}

void VertexArrayConfig::PushAttributes(std::initializer_list<uint8_t> values)
{
  for (int i = numAttrs; i < values.size(); i++, numAttrs++)
    layout[i] = *(values.begin() + i);
}

/* -----------------------------------------------------
 *          VertexArray class
 * -----------------------------------------------------
*/

VertexArray::VertexArray()
{
  _vao = 0;
  _vbo = 0;
  _ebo = 0;
  numVertices = 0; 
  numIndices = 0;
  vboSize = 0;
}

void VertexArray::InitVertexArray(VertexArrayData& data, VertexArrayConfig& config)
{
  this->_config = config;
  vboSize = data.vertDataSize;

  glGenVertexArrays(1, &_vao);
  glGenBuffers(1, &_vbo);
  glGenBuffers(1, &_ebo);

  /* Use vao */
  glBindVertexArray(_vao);
  
  /* Initialize vbo */
  glBindBuffer(GL_ARRAY_BUFFER, _vbo);
  glBufferData(GL_ARRAY_BUFFER, data.vertDataSize, data.vertData, GL_STATIC_DRAW);

  if (data.indDataSize > 0)
  {
    /* Initialize ebo */
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.indDataSize, data.indData, GL_STATIC_DRAW);
    numIndices = data.indDataSize / sizeof(uint32_t);
  }

  int stride = std::accumulate(config.layout.begin(), config.layout.end(), 0);
  int offset = 0;
  for (uint32_t i = 0; i < config.numAttrs; i++)
  {
    /* Set vao attributes */
    glVertexAttribPointer(i, config.layout[i], GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*) offset);
    glEnableVertexAttribArray(i);
    offset += config.layout[i] * sizeof(float);
  }
  numVertices = data.vertDataSize / (stride * sizeof(float));

  glBindVertexArray(0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexArray::CopyVertexBufferData(uint32_t writeBuffer)
{
  if (vboSize == 0)
    return;

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
    return;

  glBindBuffer(GL_COPY_READ_BUFFER, _ebo);
  glBindBuffer(GL_COPY_WRITE_BUFFER, writeBuffer);
  glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, indexBufferSize);
  glBindBuffer(GL_COPY_READ_BUFFER, 0);
  glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
}

void VertexArray::DestroyVertexArray() const
{
  glDeleteVertexArrays(1, &_vao);
  glDeleteBuffers(1, &_vbo);
  glDeleteBuffers(1, &_ebo);
}

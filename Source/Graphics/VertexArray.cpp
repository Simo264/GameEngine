#include "VertexArray.hh"
#include "../Logger.hh"

/* -----------------------------------------------------
 *          VertexArrayConfig class
 * -----------------------------------------------------
*/

void VertexArrayConfig::PushAttribute(uint8_t attribute)
{
  if (numAttrs >= layout.size())
  {
    CONSOLE_ERROR("VAConfiguration::pushAttribute can't push more attributes");
    return;
  }
  layout[numAttrs++] = attribute;
}


/* -----------------------------------------------------
 *          VertexArray class
 * -----------------------------------------------------
*/

void VertexArray::InitVertexArray(VertexArrayData& data, VertexArrayConfig& config)
{
  glGenVertexArrays(1, &_vao);
  glBindVertexArray(_vao);

  glGenBuffers(1, &_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, _vbo);
  glBufferData(GL_ARRAY_BUFFER, data.vertDataSize, data.vertData, GL_STATIC_DRAW);

  if (data.indDataSize > 0)
  {
    glGenBuffers(1, &_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.indDataSize, data.indData, GL_STATIC_DRAW);
    numIndices = data.indDataSize / sizeof(uint32_t);
  }

  int stride = std::accumulate(config.layout.begin(), config.layout.end(), 0);
  int offset = 0;
  for (uint32_t i = 0; i < config.numAttrs; i++)
  {
    glVertexAttribPointer(i, config.layout[i], GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*) offset);
    glEnableVertexAttribArray(i);
    offset += config.layout[i] * sizeof(float);
  }

  glBindVertexArray(0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  numVertices = data.vertDataSize / (stride * sizeof(float));
}

void VertexArray::DestroyVertexArray()
{
  glDeleteVertexArrays(1, &_vao);
  glDeleteBuffers(1, &_vbo);

  if(_ebo != 0)
    glDeleteBuffers(1, &_ebo);
}

#include "VertexArray.hh"

#include <spdlog/spdlog.h>

namespace Graphics
{

  // VAConfiguration
  // ------------------------------------------------
  VAConfiguration::VAConfiguration()
  {
    layout.fill(0);
    numAttrs = 0;
  }

  void VAConfiguration::PushAttribute(uint32_t attribute)
  {
    if (numAttrs >= layout.size())
    {
      spdlog::error("VAConfiguration::pushAttribute can't push more attributes");
      return;
    }

    layout[numAttrs++] = attribute;
  }

  // VertexArray
  // ------------------------------------------------
  VertexArray::VertexArray()
  {
    vao = -1;
    vbo = -1;
    ebo = -1;
    numIndices = -1;
  }

  void VertexArray::Init(uint32_t vertSize, float* vertData, VAConfiguration& config)
  {
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertSize, vertData, GL_STATIC_DRAW);

    int stride = std::accumulate(config.layout.begin(), config.layout.end(), 0);
    int offset = 0;
    for (uint32_t i = 0; i < config.numAttrs; i++)
    {
      glVertexAttribPointer(i, config.layout[i], GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)offset);
      glEnableVertexAttribArray(i);
      offset += config.layout[i] * sizeof(float);
    }

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    numVertices = vertSize / (stride * sizeof(float));
  }
  
  void VertexArray::Init(uint32_t vertSize, float* vertData, uint32_t indSize, uint32_t* indData, VAConfiguration& config)
  {
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertSize, vertData, GL_STATIC_DRAW);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indSize, indData, GL_STATIC_DRAW);
    

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

    numIndices  = indSize / sizeof(uint32_t);
    numVertices = vertSize / (stride * sizeof(float));
  }

  void VertexArray::Destroy()
  {
    if(vao > 0)
      glDeleteVertexArrays(1, &vao);
    if(vbo > 0)
      glDeleteBuffers(1, &vbo);
    if(ebo > 0)
      glDeleteBuffers(1, &ebo);
  }
}
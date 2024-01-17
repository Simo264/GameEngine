#include "InstancingMesh.hh"
#include "Mesh.hh"
#include "../Texture2D.hh"
#include "../Renderer.hh"
#include "../Logger.hh"

/* --------------------------------------
            PUBLIC METHODS
   -------------------------------------- */

InstancingMesh::InstancingMesh(Mesh* mesh, uint32_t nMaxInstances)
{
  _maxNumInstances = nMaxInstances;
  _nInstancesToDraw = 0;
  _instanceBuffer = 0;
  _vertexArray = nullptr;
  diffuse = nullptr;
  specular = nullptr;

  InitializeInstance(mesh);
}

void InstancingMesh::AddInstance(const Mat4f& instanceModel)
{
  if (_nInstancesToDraw >= _maxNumInstances)
  {
    CONSOLE_WARN("InstancingMesh::AddInstance can't add more instances");
    return;
  }
  SetInstanceModel(_nInstancesToDraw, instanceModel);
  _nInstancesToDraw++;
}

void InstancingMesh::SetInstanceModel(uint32_t instance, const Mat4f& model)
{
  _vertexArray->BindVertexArray();
  glBindBuffer(GL_ARRAY_BUFFER, _instanceBuffer);
  glBufferSubData(GL_ARRAY_BUFFER, instance * sizeof(Mat4f), sizeof(Mat4f), &model);
  _vertexArray->UnbindVertexArray();
}

void InstancingMesh::Draw()
{
  if (_nInstancesToDraw <= 0)
    return;

  if (diffuse)
  {
    glActiveTexture(GL_TEXTURE0);
    diffuse->BindTexture();
  }
  if (specular)
  {
    glActiveTexture(GL_TEXTURE1);
    specular->BindTexture();
  }
  if(_vertexArray->numIndices == 0)
    Renderer::DrawArraysInstanced(_vertexArray, _nInstancesToDraw);
  else
    Renderer::DrawIndexedInstanced(_vertexArray, _nInstancesToDraw);

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, 0); // unbind specular

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, 0); // unbind diffuse
}

void InstancingMesh::DestroyInstance() const
{
  //_vertexArray->DestroyVertexArray();
  glDeleteBuffers(1, &_instanceBuffer);
}

/* --------------------------------------
            PRIVATE METHODS
   -------------------------------------- */

void InstancingMesh::InitializeInstance(Mesh* mesh)
{
  _vertexArray = &mesh->vertexArray;
  _vertexArray->BindVertexArray();

  glGenBuffers(1, &_instanceBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, _instanceBuffer);
  glBufferData(GL_ARRAY_BUFFER, _maxNumInstances * sizeof(Mat4f), nullptr, GL_DYNAMIC_DRAW);

  const auto& config = mesh->vertexArray.GetConfig();
  for (int i = 0; i < 4; i++)
  {
    glVertexAttribPointer(config.numAttrs + i, 4, GL_FLOAT, GL_FALSE, sizeof(Mat4f), reinterpret_cast<void*>(i * sizeof(Vec4f)));
    glEnableVertexAttribArray(config.numAttrs + i);
    glVertexAttribDivisor(config.numAttrs + i, 1);
  }
}
#include "Shader.hpp"
#include "Core/Log/Logger.hpp"

#include "Core/Platform/OpenGL/OpenGL.hpp"


#if 0



void Shader::SetBool(const char* name, bool value) const
{
  glUniform1i(glGetUniformLocation(_shaderID, name), (int)value);
}

void Shader::SetInt(const char* name, int value) const
{
  glUniform1i(glGetUniformLocation(_shaderID, name), value);
}

void Shader::SetFloat(const char* name, float value) const
{
  glUniform1f(glGetUniformLocation(_shaderID, name), value);
}

void Shader::SetVec2f(const char* name, const Vec2f& value) const
{
  glUniform2fv(glGetUniformLocation(_shaderID, name), 1, &value[0]);
}

void Shader::SetVec3f(const char* name, const Vec3f& value) const
{
  glUniform3fv(glGetUniformLocation(_shaderID, name), 1, &value[0]);
}

void Shader::SetVec4f(const char* name, const Vec4f& value) const
{
  glUniform4fv(glGetUniformLocation(_shaderID, name), 1, &value[0]);
}

void Shader::SetMat2f(const char* name, const Mat2f& mat) const
{
  glUniformMatrix2fv(glGetUniformLocation(_shaderID, name), 1, GL_FALSE, &mat[0][0]);
}

void Shader::SetMat3f(const char* name, const Mat3f& mat) const
{
  glUniformMatrix3fv(glGetUniformLocation(_shaderID, name), 1, GL_FALSE, &mat[0][0]);
}

void Shader::SetMat4f(const char* name, const Mat4f& mat) const
{
  glUniformMatrix4fv(glGetUniformLocation(_shaderID, name), 1, GL_FALSE, &mat[0][0]);
}

void Shader::SetIntArray(const char* name, uint32_t count, int* value)
{
  glUniform1iv(glGetUniformLocation(_shaderID, name), count, value);
}

void Shader::SetFloatArray(const char* name, uint32_t count, float* value)
{
  glUniform1fv(glGetUniformLocation(_shaderID, name), count, value);
}
#endif

/* ------------------------------
      Shader object
------------------------------ */

Shader::Shader()
  : id { static_cast<uint32_t>(-1) }
{}

void Shader::Create(int shaderType)
{
  id = glCreateShader(shaderType);
}

void Shader::Delete()
{
  glDeleteShader(id);

  id = static_cast<uint32_t>(-1);
}

void Shader::LoadSource(const char* string, int length) const
{
  glShaderSource(id, 1, &string, &length);
}

bool Shader::Compile() const
{
  glCompileShader(id);

  int success = GetParameteri(GL_COMPILE_STATUS);
  return success == GL_TRUE;
}

int Shader::GetParameteri(int name) const
{
  int param;
  glGetShaderiv(id, name, &param);
  return param;
}

string Shader::GetShaderInfo() const
{
  char log[1024];
  glGetShaderInfoLog(id, sizeof(log), nullptr, log);

  return string(log);
}

/* ------------------------------
      Program object
------------------------------ */

Program::Program()
  : id{ static_cast<uint32_t>(-1) }
{}

void Program::Create()
{
  id = glCreateProgram();
}

void Program::Delete()
{
  glDeleteProgram(id);

  id = static_cast<uint32_t>(-1);
}

void Program::AttachShader(const Shader& shader) const
{
  glAttachShader(id, shader.id);
}

void Program::DetachShader(const Shader& shader) const
{
  glDetachShader(id, shader.id);
}

bool Program::Link() const
{
  glLinkProgram(id);

  int link = GetParameteri(GL_LINK_STATUS);
  return link == GL_TRUE;
}

void Program::Use() const
{
  glUseProgram(id);
}

int Program::GetParameteri(int name) const
{
  int param;
  glGetProgramiv(id, name, &param);
  return param;
}

string Program::GetProgramInfo() const
{
  char log[1024];
  glGetProgramInfoLog(id, sizeof(log), nullptr, log);
  return string(log);
}

int Program::GetUniformLocation(const char* name) const 
{ 
  return glGetUniformLocation(id, name);
}

void Program::SetUniform1i(const char* uniformname, int value) const
{
  glProgramUniform1i(id, GetUniformLocation(uniformname), value);
}
void Program::SetUniform2i(const char* uniformname, const Vec2i32& value) const
{
  glProgramUniform2i(id, GetUniformLocation(uniformname), value.x, value.y);
}
void Program::SetUniform3i(const char* uniformname, const Vec3i32& value) const
{
  glProgramUniform3i(id, GetUniformLocation(uniformname), value.x, value.y, value.z);
}
void Program::SetUniform4i(const char* uniformname, const Vec4i32& value) const
{
  glProgramUniform4i(id, GetUniformLocation(uniformname), value.x, value.y, value.z, value.w);
}
void Program::SetUniform1f(const char* uniformname, float value) const
{
  glProgramUniform1f(id, GetUniformLocation(uniformname), value);
}
void Program::SetUniform2f(const char* uniformname, const Vec2f& value) const
{
  glProgramUniform2f(id, GetUniformLocation(uniformname), value.x, value.y);
}
void Program::SetUniform3f(const char* uniformname, const Vec3f& value) const
{
  glProgramUniform3f(id, GetUniformLocation(uniformname), value.x, value.y, value.z);
}
void Program::SetUniform4f(const char* uniformname, const Vec4f& value) const
{ 
  glProgramUniform4f(id, GetUniformLocation(uniformname), value.x, value.y, value.z, value.w);
}
void Program::SetUniformMat2f(const char* uniformname, const Mat2f& value, bool transpose) const
{
  glProgramUniformMatrix2fv(id, GetUniformLocation(uniformname), 1, transpose, &value[0][0]);
}
void Program::SetUniformMat3f(const char* uniformname, const Mat3f& value, bool transpose) const
{
  glProgramUniformMatrix3fv(id, GetUniformLocation(uniformname), 1, transpose, &value[0][0]);
}
void Program::SetUniformMat4f(const char* uniformname, const Mat4f& value, bool transpose) const
{
  glProgramUniformMatrix4fv(id, GetUniformLocation(uniformname), 1, transpose, &value[0][0]);
}


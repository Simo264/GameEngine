#include "Shader.hpp"

#include "Core/Containers.hpp"
#include "Utils/Logger.hpp"

#include <glad/gl.h>

// ------------------------------
//      Shader
// ------------------------------

void Shader::Create(ShaderType type, StringView source)
{
  id = glCreateShader(static_cast<i32>(type));

  auto ptr = source.data();
  auto size = static_cast<i32>(source.size());
  glShaderSource(id, 1, &ptr, &size);
}

void Shader::Release()
{
  glDeleteShader(id);
  id = INVALID_SHADER_ID;
}

bool Shader::Compile() const
{
  glCompileShader(id);
  auto success = GetParameteri(GL_COMPILE_STATUS);
  return success == GL_TRUE;
}

i32 Shader::GetParameteri(i32 name) const
{
  auto param = 0;
  glGetShaderiv(id, name, &param);
  return param;
}

const char* Shader::GetShaderInfo() const
{
  static auto log = Array<char, 1024>{};
  log.fill(0);

  glGetShaderInfoLog(id, log.size(), nullptr, log.data());
  return log.data();
}

// ------------------------------
//      Program
// ------------------------------

void Program::Create()
{
  id = glCreateProgram();
}

void Program::Release()
{
  glDeleteProgram(id);
  id = INVALID_PROGRAM_ID;
}

void Program::AttachShader(Shader shader) const
{
  glAttachShader(id, shader.id);
}

void Program::DetachShader(Shader shader) const
{
  glDetachShader(id, shader.id);
}

bool Program::Link() const
{
  glLinkProgram(id);
  auto link = GetParameteri(GL_LINK_STATUS);
  return link == GL_TRUE;
}

void Program::Use() const
{
  glUseProgram(id);
}

i32 Program::GetParameteri(i32 name) const
{
  auto param = 0;
  glGetProgramiv(id, name, &param);
  return param;
}

const char *Program::GetProgramInfo() const
{
  static auto log = Array<char, 1024>{};
  log.fill(0);

  glGetProgramInfoLog(id, log.size(), nullptr, log.data());
  return log.data();
}

i32 Program::GetUniformLocation(StringView name) const
{
  return glGetUniformLocation(id, name.data());
}
u32 Program::GetUniformBlockIndex(StringView name) const
{
  return glGetUniformBlockIndex(id, name.data());
}

void Program::SetUniformBlockBinding(StringView blockname, i32 uniformBlockBinding) const
{
  auto blockIndex = GetUniformBlockIndex(blockname);
  if (blockIndex == GL_INVALID_INDEX)
  {
    CONSOLE_WARN("Program {}: invalid uniform block index '{}'", id, blockname);
    return;
  }

  glUniformBlockBinding(id, blockIndex, uniformBlockBinding);
}
void Program::SetUniform1i(StringView uniformname, i32 value) const
{
  auto loc = GetUniformLocation(uniformname);
  if (loc == -1)
  {
    CONSOLE_WARN("Program {}: '{}' is not an active uniform variable", id, uniformname);
    return;
  }

  glProgramUniform1i(id, loc, value);
}
void Program::SetUniform2i(StringView uniformname, const Vec2I &value) const
{
  auto loc = GetUniformLocation(uniformname);
  if (loc == -1)
  {
    CONSOLE_WARN("Program {}: '{}' is not an active uniform variable", id, uniformname);
    return;
  }

  glProgramUniform2i(id, loc, value.x, value.y);
}
void Program::SetUniform3i(StringView uniformname, const Vec3I &value) const
{
  auto loc = GetUniformLocation(uniformname);
  if (loc == -1)
  {
    CONSOLE_WARN("Program {}: '{}' is not an active uniform variable", id, uniformname);
    return;
  }

  glProgramUniform3i(id, loc, value.x, value.y, value.z);
}
void Program::SetUniform4i(StringView uniformname, const Vec4I &value) const
{
  auto loc = GetUniformLocation(uniformname);
  if (loc == -1)
  {
    CONSOLE_WARN("Program {}: '{}' is not an active uniform variable", id, uniformname);
    return;
  }

  glProgramUniform4i(id, loc, value.x, value.y, value.z, value.w);
}
void Program::SetUniform1f(StringView uniformname, f32 value) const
{
  auto loc = GetUniformLocation(uniformname);
  if (loc == -1)
  {
    CONSOLE_WARN("Program {}: '{}' is not an active uniform variable", id, uniformname);
    return;
  }

  glProgramUniform1f(id, loc, value);
}
void Program::SetUniform2f(StringView uniformname, const Vec2F &value) const
{
  auto loc = GetUniformLocation(uniformname);
  if (loc == -1)
  {
    CONSOLE_WARN("Program {}: '{}' is not an active uniform variable", id, uniformname);
    return;
  }

  glProgramUniform2f(id, loc, value.x, value.y);
}
void Program::SetUniform3f(StringView uniformname, const Vec3F &value) const
{
  auto loc = GetUniformLocation(uniformname);
  if (loc == -1)
  {
    CONSOLE_WARN("Program {}: '{}' is not an active uniform variable", id, uniformname);
    return;
  }

  glProgramUniform3f(id, loc, value.x, value.y, value.z);
}
void Program::SetUniform4f(StringView uniformname, const Vec4F &value) const
{
  auto loc = GetUniformLocation(uniformname);
  if (loc == -1)
  {
    CONSOLE_WARN("Program {}: '{}' is not an active uniform variable", id, uniformname);
    return;
  }

  glProgramUniform4f(id, loc, value.x, value.y, value.z, value.w);
}
void Program::SetUniformMat2f(StringView uniformname, const Mat2F &value, bool transpose) const
{
  auto loc = GetUniformLocation(uniformname);
  if (loc == -1)
  {
    CONSOLE_WARN("Program {}: '{}' is not an active uniform variable", id, uniformname);
    return;
  }

  glProgramUniformMatrix2fv(id, loc, 1, transpose, &value[0][0]);
}
void Program::SetUniformMat3f(StringView uniformname, const Mat3F &value, bool transpose) const
{
  auto loc = GetUniformLocation(uniformname);
  if (loc == -1)
  {
    CONSOLE_WARN("Program {}: '{}' is not an active uniform variable", id, uniformname);
    return;
  }

  glProgramUniformMatrix3fv(id, loc, 1, transpose, &value[0][0]);
}
void Program::SetUniformMat4f(StringView uniformname, const Mat4F &value, bool transpose) const
{
  auto loc = GetUniformLocation(uniformname);
  if (loc == -1)
  {
    CONSOLE_WARN("Program {}: '{}' is not an active uniform variable", id, uniformname);
    return;
  }

  glProgramUniformMatrix4fv(id, loc, 1, transpose, &value[0][0]);
}


void Program::SetUniform1i(i32 loc, i32 value) const
{
  glProgramUniform1i(id, loc, value);
}
void Program::SetUniform2i(i32 loc, const Vec2I& value) const
{
  glProgramUniform2i(id, loc, value.x, value.y);
}
void Program::SetUniform3i(i32 loc, const Vec3I& value) const
{
  glProgramUniform3i(id, loc, value.x, value.y, value.z);
}
void Program::SetUniform4i(i32 loc, const Vec4I& value) const
{
  glProgramUniform4i(id, loc, value.x, value.y, value.z, value.w);
}
void Program::SetUniform1f(i32 loc, f32 value) const
{
  glProgramUniform1f(id, loc, value);
}
void Program::SetUniform2f(i32 loc, const Vec2F& value) const
{
  glProgramUniform2f(id, loc, value.x, value.y);
}
void Program::SetUniform3f(i32 loc, const Vec3F& value) const
{
  glProgramUniform3f(id, loc, value.x, value.y, value.z);
}
void Program::SetUniform4f(i32 loc, const Vec4F& value) const
{
  glProgramUniform4f(id, loc, value.x, value.y, value.z, value.w);
}
void Program::SetUniformMat2f(i32 loc, const Mat2F& value, bool transpose) const
{
  glProgramUniformMatrix2fv(id, loc, 1, transpose, &value[0][0]);
}
void Program::SetUniformMat3f(i32 loc, const Mat3F& value, bool transpose) const
{
  glProgramUniformMatrix3fv(id, loc, 1, transpose, &value[0][0]);
}
void Program::SetUniformMat4f(i32 loc, const Mat4F& value, bool transpose) const
{
  glProgramUniformMatrix4fv(id, loc, 1, transpose, &value[0][0]);
}

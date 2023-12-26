#pragma once

#include "../Core.hh"

class Shader
{
public:
  Shader() : _shaderID{ 0 } { }
  ~Shader() = default;

  Shader(const Shader&) = delete;            // delete copy constructor
  Shader& operator=(const Shader&) = delete; // delete assign op

  void Create(const char* label, const char* vertFilePath, const char* fragFilePath);
  void Use() const { glUseProgram(_shaderID); }
  void Destroy() { glDeleteProgram(_shaderID); }
  const string& Label() const { return _label; }

  void SetBool(const char* name, bool value) const;
  void SetInt(const char* name, int value) const;
  void SetFloat(const char* name, float value) const;

  void SetVec2f(const char* name, const vec2f& value) const;
  void SetVec3f(const char* name, const vec3f& value) const;
  void SetVec4f(const char* name, const vec4f& value) const;
  void SetMat2f(const char* name, const mat2f& mat) const;
  void SetMat3f(const char* name, const mat3f& mat) const;
  void SetMat4f(const char* name, const mat4f& mat) const;

private:
  uint32_t _shaderID;
  string   _label;

  void GetSourceFromFile(const char* filePath, string& dest);
  bool Compile(uint32_t& shader, const char* src);
  bool CreateAndLink(uint32_t vShader, uint32_t fShader);
};

#pragma once

#include "../Core.hh"

class Shader
{
public:
  Shader() : _shaderID{ 0 } { }
  ~Shader() = default;

  Shader(const Shader&) = delete;            // delete copy constructor
  Shader& operator=(const Shader&) = delete; // delete assign op

  void Create(const char* label, const char* vertFile, const char* fragFile);
  void Use() const { glUseProgram(_shaderID); }
  void Destroy() { glDeleteProgram(_shaderID); }
  const String& Label() const { return _label; }

  void SetBool(const char* name, bool value) const;
  void SetInt(const char* name, int value) const;
  void SetFloat(const char* name, float value) const;

  void SetVec2f(const char* name, const Vec2f& value) const;
  void SetVec3f(const char* name, const Vec3f& value) const;
  void SetVec4f(const char* name, const Vec4f& value) const;
  void SetMat2f(const char* name, const Mat2f& mat) const;
  void SetMat3f(const char* name, const Mat3f& mat) const;
  void SetMat4f(const char* name, const Mat4f& mat) const;

private:
  uint32_t _shaderID;
  String   _label;

  void GetSourceFromFile(const char* sourceFile, String& dest);
  bool Compile(uint32_t& shader, const char* src);
  bool CreateAndLink(uint32_t vShader, uint32_t fShader);
};

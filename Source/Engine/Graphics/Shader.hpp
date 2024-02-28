#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Math.hpp"

/* -----------------------------------
      Shader program class
  ----------------------------------- */
class Shader
{
public:
  Shader(const char* label, const Path& vertFile, const Path& fragFile);

  /* Disable copy constructor */
  Shader(Shader const&) = delete;
  Shader& operator=(Shader const&) = delete;

  void Use() const; 
  void DestroyShader();
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

  void SetIntArray(const char* name, uint32_t count, int* value);
  void SetFloatArray(const char* name, uint32_t count, float* value);

private:
  uint32_t _shaderID;
  String   _label;

  void GetSourceFromFile(const Path& sourceFile, String& dest);
  bool Compile(uint32_t& shader, const char* src);
  bool CreateAndLink(uint32_t vShader, uint32_t fShader);
};

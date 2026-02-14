#pragma once

#include "Core/Types.hpp"
#include "Core/Strings.hpp"
#include "Core/Math/Types.hpp"

enum class ShaderType : i32
{
  Vertex = 0x8B31, // GL_VERTEX_SHADER
  TessControl = 0x8E88, // GL_TESS_CONTROL_SHADER
  TessEvaluation = 0x8E87, // GL_TESS_EVALUATION_SHADER
  Geometry = 0x8DD9, // GL_GEOMETRY_SHADER
  Fragment = 0x8B30  // GL_FRAGMENT_SHADER
};

using ShaderId = u32;
constexpr auto INVALID_SHADER_ID = static_cast<u32>(-1);

using ProgramId = u32;
constexpr auto INVALID_PROGRAM_ID = static_cast<ProgramId>(-1);

/**
 * @brief https://registry.khronos.org/OpenGL/specs/gl/glspec46.core.pdf (chapter 7)
 * 
 * A shader specifies operations that are meant to occur on data as it moves
 * through different programmable stages of the OpenGL processing pipeline, 
 * starting with vertices specified by the application and ending with fragments prior to
 * being written to the framebuffer. The programming language used for shaders is
 * described in the OpenGL Shading Language Specification.
 */
struct Shader
{
public:
  Shader() : id{ INVALID_SHADER_ID }{}

  /** @brief Creates an empty shader object and returns a non-zero value by which it can be referenced.*/
  void Create(ShaderType type, StringView source);
  /** @brief Frees the memory and invalidates the name associated with the shader object */
  void Release();
  /** @brief Compiles the source code strings that have been stored in the shader object. */
  bool Compile() const;
  /** @return A parameter from a shader object. */
  i32 GetParameteri(i32 name) const;
  /** @return The information log for the specified shader object.*/
  const char* GetShaderInfo() const;
  
  auto Valid() const { return id != INVALID_SHADER_ID; }

  ShaderId id;
};

/**
 * @brief One or more shader objects are attached to a program object. 
 * The program object is then linked, which generates executable code from all the compiled shader
 * objects attached to the program.
 * 
 * When program objects are bound to a shader stage, they become the current
 * program object for that stage. When the current program object for a shader stage
 * includes a shader of that type, it is considered the active program object for that stage.
 * 
 * The current program object for all stages may be set at once using a single unified program object,
 * or the current program object may be set for each stage individually using a separable program object
 * where different separable program objects may be current for other stages.
 * 
 * The set of separable program objects current for all stages are collected in a program pipeline object
 * that must be bound for use. 
 * When a linked program object is made active for one of the stages, the corresponding executable code
 * is used to perform processing for that stage.
 */
struct Program
{
public:
  Program() : id{ INVALID_PROGRAM_ID } {}

  /** @brief Creates an empty program object and returns a non-zero value by which it can be referenced. */
  void Create();
  /** @brief Frees the memory and invalidates the name associated with the program object */
  void Release();
  /** @brief Attaches a shader object to the program object */
  void AttachShader(Shader shader) const;
  /** @brief Detaches the shader object specified from the program object */
  void DetachShader(Shader shader) const;
  /** @brief Links the program object specified. */
  bool Link() const;
  /** @return A parameter from the program object. See https://registry.khronos.org/OpenGL-Refpages/gl4/html/glGetProgram.xhtml */
  i32 GetParameteri(i32 name) const;
  /** @return The information log for the program object */
  const char* GetProgramInfo() const;
  /** @brief Installs the program object specified as part of current rendering state. */
  void Use() const;
  /** @return The location of a uniform variable */
  i32 GetUniformLocation(StringView name) const;
  /** @return The index of a named uniform block */
  u32 GetUniformBlockIndex(StringView name) const;
  /** @brief Assign a binding poi32 to an active uniform block */
  void SetUniformBlockBinding(StringView blockname, i32 uniformBlockBinding) const;
  /** @brief Specify the value of a uniform variable for the program object */
  void SetUniform1i(i32 loc, i32 value) const;
  void SetUniform2i(i32 loc, const Vec2I& value) const;
  void SetUniform3i(i32 loc, const Vec3I& value) const;
  void SetUniform4i(i32 loc, const Vec4I& value) const;
  void SetUniform1f(i32 loc, f32 value) const;
  void SetUniform2f(i32 loc, const Vec2F& value) const;
  void SetUniform3f(i32 loc, const Vec3F& value) const;
  void SetUniform4f(i32 loc, const Vec4F& value) const;
  void SetUniformMat2f(i32 loc, const Mat2F& value, bool transpose = false) const;
  void SetUniformMat3f(i32 loc, const Mat3F& value, bool transpose = false) const;
  void SetUniformMat4f(i32 loc, const Mat4F& value, bool transpose = false) const;

  void SetUniform1i(StringView uniformname, i32 value) const;
  void SetUniform2i(StringView uniformname, const Vec2I& value) const;
  void SetUniform3i(StringView uniformname, const Vec3I& value) const;
  void SetUniform4i(StringView uniformname, const Vec4I& value) const;
  void SetUniform1f(StringView uniformname, f32 value) const;
  void SetUniform2f(StringView uniformname, const Vec2F& value) const;
  void SetUniform3f(StringView uniformname, const Vec3F& value) const;
  void SetUniform4f(StringView uniformname, const Vec4F& value) const;
  void SetUniformMat2f(StringView uniformname, const Mat2F& value, bool transpose = false) const;
  void SetUniformMat3f(StringView uniformname, const Mat3F& value, bool transpose = false) const;
  void SetUniformMat4f(StringView uniformname, const Mat4F& value, bool transpose = false) const;

  auto Valid() const { return id != INVALID_PROGRAM_ID; }

  ProgramId id;
};

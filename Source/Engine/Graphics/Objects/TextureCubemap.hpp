#pragma once

#include "Core/Core.hpp"

class TextureCubemap
{
public:
	TextureCubemap() : 
    id{ 0 } 
  {}
	~TextureCubemap() = default;

	/**
	 * @brief
	 * Create texture object
	 */
	void Create();

  /**
   * @brief
   * Allocate the entire cubemap with a single call glTextureStorage2D.
   * See https://registry.khronos.org/OpenGL-Refpages/gl4/html/glTexStorage2D.xhtml
   */
  void CreateStorage(i32 internalFormat, i32 width, i32 height) const;

  /**
   * @brief 
   * Specify a three-dimensional texture subimage. 
   * See https://registry.khronos.org/OpenGL-Refpages/gl4/html/glTexSubImage3D.xhtml
   */
  void SubImage3D(
    i32 level,
    i32 xoffset,
    i32 yoffset,
    i32 zoffset,
    i32 width,
    i32 height,
    i32 depth,
    i32 format,
    i32 type,
    const void* pixels) const;

  /**
   * @brief
   * Delete texture object and invalidates the name associated with the texture object
   */
  void Delete();

  /**
   * @brief
   * Bind the texture object to the specified texture unit
   *
   * @param unit: specifies the texture unit, to which the texture object should be bound to
   */
  void BindTextureUnit(i32 unit) const;

  /**
   * @brief
   * Set texture parameters
   */
  void SetParameteri(i32 name, i32 value) const;
  void SetParameteriv(i32 name, i32* values) const;
  void SetParameterf(i32 name, f32 value) const;
  void SetParameterfv(i32 name, f32* values) const;

  void LoadImages(const Array<class Texture2D*, 6>& images) const;

  bool IsValid() const;

	u32 id;
};
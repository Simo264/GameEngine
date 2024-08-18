#pragma once

#include "Core/Core.hpp"

class TextureCubemap
{
public:
	TextureCubemap();
	~TextureCubemap() = default;

	/**
	 * Create texture object
	 */
	void Create();

  /**
   * Allocate the entire cubemap with a single call glTextureStorage2D
   *
   * @param width: specifies the width of the texture, in texels.
   * @param height: specifies the height of the texture, in texels.
   */
  void CreateStorage(i32 internalFormat, i32 width, i32 height) const;

  /**
   * Specify a three-dimensional texture subimage
   *
   * @param level: specifies the level-of-detail number; 0 is the base image level. Level n is the nth mipmap reduction image
   * @param xoffset: specifies a texel offset in the x direction within the texture array
   * @param yoffset: specifies a texel offset in the y direction within the texture array.
   * @param zoffset: specifies a texel offset in the z direction within the texture array.
   * @param width:  specifies the width of the texture subimage
   * @param height: specifies the height of the texture subimage
   * @param depth:  specifies the depth of the texture subimage
   * @param format: specifies the format of the pixel data. The following symbolic values are accepted:
   *                GL_RED, GL_RG, GL_RGB, GL_BGR, GL_RGBA, GL_DEPTH_COMPONENT, and GL_STENCIL_INDEX.
   * @param type: specifies the data type of the pixel data
   * @param pixels: specifies a pointer to the image data in memory
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
   * Delete texture object and invalidates the name associated with the texture object
   */
  void Delete();

  /**
   * Bind texture object
   */
  void Bind() const;

  /**
   * Unbind texture object
   */
  void Unbind() const;

  /**
   * Bind the texture object to the specified texture unit
   *
   * @param unit: specifies the texture unit, to which the texture object should be bound to
   */
  void BindTextureUnit(i32 unit) const;

  /**
   * Set texture parameters
   */
  void SetParameteri(i32 name, i32 value) const;
  void SetParameteriv(i32 name, int* values) const;
  void SetParameterf(i32 name, f32 value) const;
  void SetParameterfv(i32 name, f32* values) const;

  void LoadImages(const Array<class Texture2D*, 6>& images) const;

	u32 id;

	const i32 target;
};